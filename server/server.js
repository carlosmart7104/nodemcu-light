const express = require('express');
const http = require('http');
const WebSocket = require('ws');

var ipaddress = process.env.OPENSHIFT_NODEJS_IP;
var port = process.env.OPENSHIFT_NODEJS_PORT || 80;

if (typeof ipaddress === "undefined") {
  console.warn('No OPENSHIFT_NODEJS_IP var, using 0.0.0.0');
  ipaddress = "0.0.0.0";
};

const app = express();

app.use(express.static('public'));

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

wss.on('connection', function connection(ws) {

  ws.on('message', function incoming(message) {
    console.log(message);
    wss.clients.forEach(function each(client) {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
  });

});

app.use(function (req, res) {
  res.send({ msg: "hola soy un servidor" });
});

server.listen(port, ipaddress, function listening() {
  console.log('Listening on localhost');
});