var net = require('net');
var io = require('socket.io')
var http = require('http')
var url = require('url')
var fs = require('fs')
var HOST = '127.0.0.1';
var PORT = 6969;

// Create a server instance, and chain the listen function to it
// The function passed to net.createServer() becomes the event handler for the 'connection' event
// The sock object the callback function receives UNIQUE for each connection


var server = http.createServer(
		function(request, response) {
		console.log('Connection');
		var path = url.parse(request.url).pathname;

		switch (path) {
		case '/':
		response.writeHead(200, {'Content-Type': 'text/html'});
		response.write('Hello, World.');
		response.end();
		break;
		case '/index.html':
		fs.readFile(__dirname + path, function(error, data) {
				if (error){
				response.writeHead(404);
				response.write("opps this doesn't exist - 404");
				} else {
				response.writeHead(200, {"Content-Type": "text/html"});
				response.write(data, "utf8");
				}
				response.end();
				});
		break;
		default:
		response.writeHead(404);
		response.write("opps this doesn't exist - 404");
		response.end();
		break;
		}
		}

);

server.listen(8001);
var ser_io = io.listen(server);
var update = 0;
var ss = "original";
var json_current_state = {
		"bank": { "max": 0, "cur": 0 },
		"cus1": { "max": -1, "cur": 0 },
		"cus2": { "max": -1, "cur": 0 },
		"cus3": { "max": -1, "cur": 0 },
		"cus4": { "max": -1, "cur": 0 },
		"cus5": { "max": -1, "cur": 0 },
		"cus6": { "max": -1, "cur": 0 },
		"cus7": { "max": -1, "cur": 0 },
		"cus8": { "max": -1, "cur": 0 },
		"cus9": { "max": -1, "cur": 0 },
		"cus10": { "max": -1, "cur": 0 },
		"cus11": { "max": -1, "cur": 0 },
		"cus12": { "max": -1, "cur": 0 },
		"cus13": { "max": -1, "cur": 0 },
		"cus14": { "max": -1, "cur": 0 },
		"cus15": { "max": -1, "cur": 0 },
		"cus16": { "max": -1, "cur": 0 },
		"cus17": { "max": -1, "cur": 0 },
		"cus18": { "max": -1, "cur": 0 },
		"cus19": { "max": -1, "cur": 0 },
		"cus20": { "max": -1, "cur": 0 }
	};

var gloSock = null;

ser_io.sockets.on('connection', function(socket) {
		setInterval(function() {
				socket.emit('date', {'date': new Date()});
		}, 1000);

		setInterval(
			function(){
				if(update == 1){
					//socket.emit('update', {'val': new Number(ss)});
					socket.emit('update', {'json': json_current_state});
					update = 0;
				}
			}
		, 50);

		// 接收來自於瀏覽器的資料
		socket.on('client_data', function(data) {
				process.stdout.write(data.letter);
		});
});


net.createServer(function(sock) {

    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

    // Add a 'data' event handler to this instance of socket
    sock.on('data', function(data) {

        console.log('DATA ' + sock.remoteAddress + ': ' + data);
        // Write the data back to the socket, the client will receive it as data from the server
		
		json_rev = JSON.parse(data);
		sock.write(JSON.stringify(json_rev) + '\0')
		for (var who in json_rev){
			for (var att in json_rev[who]){
				json_current_state[who][att] = json_rev[who][att];
			}
		}
		console.log('\n\nJSON now: \n' + JSON.stringify(json_current_state));
		//ss = json_rev['1'];
		update = 1;
    });

    // Add a 'close' event handler to this instance of socket
    sock.on('close', function(data) {
        console.log('CLOSED: ' + sock.remoteAddress +' '+ sock.remotePort);
    });

}).listen(PORT, HOST);

console.log('Server listening on ' + HOST +':'+ PORT);
