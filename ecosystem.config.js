module.exports = {
  apps: [
    {
      name: 'christmas-mini-market',
      script: 'server.js',
      cwd: '/home/shinigami/christmas/Christmas_cgi',
      interpreter: 'node',
      autorestart: true,
      max_restarts: 30,
      restart_delay: 3000,
      max_memory_restart: '256M',
      time: true,
      env: {
        NODE_ENV: 'production',
        PORT: 8090,
      },
    },
  ],
};
