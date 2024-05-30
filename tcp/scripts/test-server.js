#!/usr/bin/env node

const http = require('http');

const server = http.createServer((req, res) => {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/html');
  res.end('<h1>Hello World</h1>');
});

server.listen(3000, '192.168.178.201', () => {
    console.log('Server running at http://192.168.178.201:3000/');
});
