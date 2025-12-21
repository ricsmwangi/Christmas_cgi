const express = require('express');
const { execSync } = require('child_process');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.urlencoded({ extended: true }));
app.use(express.json());

function executeCGI(scriptName, queryString = '') {
    try {
        const env = Object.assign({}, process.env, {
            REQUEST_METHOD: 'GET',
            QUERY_STRING: queryString
        });

        const result = execSync(`./${scriptName}`, {
            env,
            cwd: __dirname,
            encoding: 'utf-8'
        });

        return result.replace(/^Content-Type:.*\n\n?/, '');
    } catch (error) {
        console.error(`CGI Error: ${error.message}`);
        return `<h1>Error</h1><p>${error.message}</p>`;
    }
}

app.get('/', (req, res) => {
    const html = executeCGI('main.cgi');
    res.set('Content-Type', 'text/html').send(html);
});

app.get('/:action', (req, res) => {
    const { action } = req.params;
    const queryString = `action=${action}`;
    const html = executeCGI('main.cgi', queryString);
    res.set('Content-Type', 'text/html').send(html);
});

app.post('/tree', (req, res) => {
    const { height } = req.body;
    const queryString = `action=tree&height=${height || 8}`;
    const html = executeCGI('main.cgi', queryString);
    res.set('Content-Type', 'text/html').send(html);
});

app.listen(PORT, '0.0.0.0', () => {
    console.log(`🎄 Christmas CGI on port ${PORT}`);
});
