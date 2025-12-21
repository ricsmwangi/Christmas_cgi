const express = require('express');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.urlencoded({ extended: true }));
app.use(express.json());

function executeCGI(scriptName, queryString = '', postData = '', isPost = false, timeout = 5000) {
    return new Promise((resolve, reject) => {
        const env = Object.assign({}, process.env, {
            REQUEST_METHOD: isPost ? 'POST' : 'GET',
            QUERY_STRING: queryString  // Keep query string even for POST
        });

        if (isPost) {
            env.CONTENT_TYPE = 'application/x-www-form-urlencoded';
            env.CONTENT_LENGTH = Buffer.byteLength(postData);
        }

        const child = spawn(`./${scriptName}`, [], {
            env,
            cwd: __dirname,
            timeout
        });

        let output = '';
        let error = '';

        if (isPost && postData) {
            child.stdin.write(postData);
            child.stdin.end();
        }

        child.stdout.on('data', (data) => {
            output += data.toString();
        });

        child.stderr.on('data', (data) => {
            error += data.toString();
        });

        child.on('close', (code) => {
            if (code !== 0) {
                reject(new Error(`Process exited with code ${code}: ${error}`));
            } else {
                resolve(output);
            }
        });

        child.on('error', (err) => {
            reject(err);
        });
    });
}

app.get('/', async (req, res) => {
    try {
        const output = await executeCGI('main.cgi', '');
        const parts = output.split('\n\n');
        const body = parts.length > 1 ? parts.slice(1).join('\n\n') : output;
        res.set('Content-Type', 'text/html').send(body);
    } catch (error) {
        console.error('Error:', error);
        res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
    }
});

app.get('/index.html', async (req, res) => {
    try {
        const output = await executeCGI('main.cgi', '');
        const parts = output.split('\n\n');
        const body = parts.length > 1 ? parts.slice(1).join('\n\n') : output;
        res.set('Content-Type', 'text/html').send(body);
    } catch (error) {
        console.error('Error:', error);
        res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
    }
});

app.all('*', async (req, res) => {
    try {
        const url = new URL(req.url, `http://${req.get('host')}`);
        const queryString = url.searchParams.toString();
        const isPost = req.method === 'POST';
        
        // Convert req.body object to form-encoded string
        let postData = '';
        if (isPost && req.body) {
            postData = new URLSearchParams(req.body).toString();
        }
        
        const output = await executeCGI('main.cgi', queryString, postData, isPost);
        
        // Strip CGI headers (everything before first blank line)
        const parts = output.split('\n\n');
        const body = parts.length > 1 ? parts.slice(1).join('\n\n') : output;
        
        res.set('Content-Type', 'text/html').send(body);
    } catch (error) {
        console.error('Error:', error);
        res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
    }
});

app.listen(PORT, '0.0.0.0', () => {
    console.log(`🎄 Christmas CGI Server running on port ${PORT}`);
});
