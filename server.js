const express = require('express');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.urlencoded({ extended: true }));
app.use(express.json());

function executeCGI(scriptName, queryString = '', timeout = 5000) {
    return new Promise((resolve, reject) => {
        const env = Object.assign({}, process.env, {
            REQUEST_METHOD: 'GET',
            QUERY_STRING: queryString
        });

        const child = spawn(`./${scriptName}`, [], {
            env,
            cwd: __dirname,
            timeout
        });

        let output = '';
        let error = '';

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
        const html = await executeCGI('main.cgi', '');
        res.set('Content-Type', 'text/html').send(html);
    } catch (error) {
        console.error('Error:', error);
        res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
    }
});

app.get('/index.html', async (req, res) => {
    try {
        const html = await executeCGI('main.cgi', '');
        res.set('Content-Type', 'text/html').send(html);
    } catch (error) {
        console.error('Error:', error);
        res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
    }
});

app.all('*', async (req, res) => {
    try {
        const queryString = new URLSearchParams(req.query).toString();
        const html = await executeCGI('main.cgi', queryString);
        res.set('Content-Type', 'text/html').send(html);
    } catch (error) {
        console.error('Error:', error);
        res.status(500).send(`<h1>Error</h1><p>${error.message}</p>`);
    }
});

app.listen(PORT, '0.0.0.0', () => {
    console.log(`🎄 Christmas CGI Server running on port ${PORT}`);
});
