const express = require('express');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.urlencoded({ extended: true, limit: '256kb' }));
app.use(express.json({ limit: '256kb' }));

// Basic security headers
app.use((req, res, next) => {
  res.set('X-Content-Type-Options', 'nosniff');
  res.set('X-Frame-Options', 'SAMEORIGIN');
  res.set('Referrer-Policy', 'strict-origin-when-cross-origin');
  next();
});

function executeCGI(scriptName, queryString = '', postData = '', isPost = false, timeoutMs = 10000) {
  return new Promise((resolve, reject) => {
    const env = Object.assign({}, process.env, {
      REQUEST_METHOD: isPost ? 'POST' : 'GET',
      QUERY_STRING: queryString,
    });

    if (isPost) {
      env.CONTENT_TYPE = 'application/x-www-form-urlencoded';
      env.CONTENT_LENGTH = Buffer.byteLength(postData).toString();
    }

    const child = spawn(path.join(__dirname, scriptName), [], { env, cwd: __dirname });

    let output = '';
    let error = '';
    let settled = false;

    const timer = setTimeout(() => {
      if (!settled) {
        settled = true;
        child.kill('SIGKILL');
        reject(new Error('CGI process timed out'));
      }
    }, timeoutMs);

    // Always close stdin so the child never blocks waiting for input
    if (isPost && postData) {
      child.stdin.end(postData);
    } else {
      child.stdin.end();
    }

    child.stdout.on('data', (data) => { output += data.toString(); });
    child.stderr.on('data', (data) => { error += data.toString(); });

    child.on('close', (code) => {
      clearTimeout(timer);
      if (settled) return;
      settled = true;
      if (code !== 0) {
        reject(new Error(`Process exited with code ${code}: ${error || '(no stderr)'}`));
      } else {
        resolve(output);
      }
    });

    child.on('error', (err) => {
      clearTimeout(timer);
      if (!settled) {
        settled = true;
        reject(err);
      }
    });
  });
}

// Strip CGI response headers (blank-line separated), keep the body
function cgiBody(output) {
  const match = output.match(/^([^]*?)\r?\n\r?\n/);
  return match ? output.slice(match[0].length) : output;
}

// --- Christmas messages translated by translate-llamacpp (port 8087) ---
const fs = require('fs');
const PHRASES_FILE = path.join(__dirname, 'phrases.json');
const TRANSLATE_URL = process.env.TRANSLATE_URL || 'http://127.0.0.1:8087';
const TRANSLATE_MODEL = process.env.TRANSLATE_MODEL || 'translategemma:4b';
const TR_LANGS = ['French', 'Spanish', 'German', 'Italian', 'Portuguese',
  'Swahili', 'Hindi', 'Japanese', 'Korean', 'Arabic', 'Russian', 'Chinese'];

function loadPhrases() {
  try { return JSON.parse(fs.readFileSync(PHRASES_FILE, 'utf8')); }
  catch (e) { return []; }
}
function savePhrases(p) {
  try { fs.writeFileSync(PHRASES_FILE, JSON.stringify(p, null, 2)); }
  catch (e) { console.error('phrase save failed:', e.message); }
}
function updatePhrase(text, translations, done) {
  const list = loadPhrases();
  const p = list.find(x => x.text === text);
  if (p) { p.translations = translations; p.done = done; savePhrases(list); }
}

// Translate one phrase into all languages (runs sequentially, never in parallel)
let translateChain = Promise.resolve();
function queueTranslate(text) {
  translateChain = translateChain
    .then(() => translatePhrase(text))
    .catch(e => console.error('translate job failed:', e.message));
}
async function translatePhrase(text) {
  const prompt = 'Translate this Christmas message into these languages: ' + TR_LANGS.join(', ') +
    '. One line per language, exactly in the form "Language: translation", no extra text.\nMessage: ' + text;
  const ctrl = new AbortController();
  const timer = setTimeout(() => ctrl.abort(), 120000);
  try {
    const r = await fetch(TRANSLATE_URL + '/v1/chat/completions', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        model: TRANSLATE_MODEL, max_tokens: 500, temperature: 0.2,
        messages: [{ role: 'user', content: prompt }],
      }),
      signal: ctrl.signal,
    });
    const j = await r.json();
    const content = (j.choices && j.choices[0] && j.choices[0].message && j.choices[0].message.content) || '';
    const tr = {};
    content.split('\n').forEach(line => {
      const m = line.match(/^\s*([A-Za-z ]{2,30}):\s*(.+)$/);
      if (m) tr[m[1].trim().toLowerCase()] = m[2].trim();
    });
    updatePhrase(text, tr, Object.keys(tr).length > 0);
    if (Object.keys(tr).length === 0) console.error('translate parse empty for:', text);
  } catch (e) {
    console.error('translate failed:', e.message);
    updatePhrase(text, {}, true); // give up, page falls back to English
  } finally {
    clearTimeout(timer);
  }
}

app.get('/api/phrases', (req, res) => {
  res.json({ phrases: loadPhrases(), langs: TR_LANGS });
});

app.post('/api/phrases', (req, res) => {
  let text = String((req.body && req.body.text) || '').replace(/[\x00-\x1f\x7f]/g, ' ').trim();
  if (!text) return res.status(400).json({ ok: false, error: 'empty message' });
  if (text.length > 140) text = text.slice(0, 140);
  const list = loadPhrases();
  let entry = list.find(x => x.text === text);
  if (!entry) {
    entry = { text, translations: {}, done: false, added: Date.now() };
    list.push(entry);
    savePhrases(list);
    queueTranslate(text);
  }
  res.json({ ok: true, phrase: entry });
});

async function handle(req, res) {
  try {
    const isPost = req.method === 'POST';
    const url = new URL(req.url, `http://${req.get('host') || 'localhost'}`);
    const queryString = url.searchParams.toString();
    let postData = '';
    if (isPost && req.body) {
      postData = new URLSearchParams(req.body).toString();
    }
    const output = await executeCGI('main.cgi', queryString, postData, isPost);
    res.set('Content-Type', 'text/html; charset=utf-8').send(cgiBody(output));
  } catch (error) {
    console.error('Error:', error);
    res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
  }
}

app.get('/', handle);
app.get('/index.html', handle);
app.all('*', handle);

app.listen(PORT, '0.0.0.0', () => {
  console.log(`🎄 Christmas CGI Server running on port ${PORT}`);
});
