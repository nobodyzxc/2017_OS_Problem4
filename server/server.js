var net = require('net');
var io = require('socket.io')
var http = require('http')
var url = require('url')
var fs = require('fs')
var HOST = '127.0.0.1';
var PORT = 6969;
var Path = require('path');
var sockSet = new Set();
var cppServerSock;

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
            var fileType;
            if(Path.extname(path) == '.css') fileType = 'css';
            if(Path.extname(path) == '.js') fileType = 'javascript';
            else fileType = Path.extname(path).substring(1);
            fs.readFile(__dirname + path, function(error, data) {
                if (error){
                    response.writeHead(404);
                    response.write("opps this doesn't exist - 404");
                } else {
                    response.writeHead(200, {"Content-Type": "text/" + fileType});
                    response.write(data, "utf8");
                }
                response.end();
            });
            break;
        }
    }

);

server.listen(8001);
var ser_io = io.listen(server);
var update = 0;
var ss = "original";
var json_current_state = {
    "bank":  { "max": 100000 , "cur": 100000 , "wait" : 0 } ,
    "cus1":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus2":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus3":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus4":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus5":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus6":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus7":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus8":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus9":  { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus10": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus11": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus12": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus13": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus14": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus15": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus16": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus17": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus18": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus19": { "max": -1, "cur": 0 , "wait" : 0 } ,
    "cus20": { "max": -1, "cur": 0 , "wait" : 0 }
};

ser_io.sockets.on('connection', function(socket){
    socket.emit('init', {'json': json_current_state});
    sockSet.add(socket);
    // 接收來自於瀏覽器的資料
    socket.on('client_data', function(data) {
        console.log(data);
        cppServerSock.write(JSON.stringify(data));
        //cppServerSock.pipe(cppServerSock);
        //process.stdout.write(data.letter);
    });
});

ser_io.sockets.on('close', function(socket){
    socket.emit('init', {'json': json_current_state});
    sockSet.add(socket);
    // 接收來自於瀏覽器的資料
    socket.on('client_data', function(data) {
        console.log(data);
        cppServerSock.write(JSON.stringify(data));
        //cppServerSock.pipe(cppServerSock);
        //process.stdout.write(data.letter);
    });
});
/*
 setInterval(function(){
     socket.emit('date', {'date': new Date()});
 },
 1000);

 setInterval(function(){
     if(update == 1){
         //socket.emit('update', {'val': new Number(ss)});
         socket.emit('update', {'json': json_current_state});
         update = 0;
     }
 }
 , 100);

 */


/* recv C's info */
net.createServer(function(sock) {

    // We have a connection -
    // a socket object is assigned
    // to the connection automatically
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

    cppServerSock = sock;

    // Add a 'data' event handler to this instance of socket
    sock.on('data', function(data) {

        data = "[" +
        data.toString('utf8').substring(0 , data.length - 1)
        + "]";

        json_rev = JSON.parse(data);

        for(var i = 0 ; i < json_rev.length ; i++){
            for (var who in json_rev[i]){
                if(who.indexOf('-') >= 0){
                    who = who.replace('-','');
                    json_current_state[who]["wait"] = 1;
                    continue;
                }
                for (var att in json_rev[i][who]){
                    json_current_state[who][att] =
                    json_rev[i][who][att];
                }
                json_current_state[who]["wait"] = 0;
                console.log(JSON.stringify(json_rev[i]));
            }
        }

        sockSet.forEach(function (s) {
            s.emit('update', json_rev);
        });
    });

    // Add a 'close' event handler to this instance of socket
    sock.on('close', function(data) {
        console.log('CLOSED: ' +
        sock.remoteAddress + ' ' + sock.remotePort);
        cppServerSock = undefined;
    });

}).listen(PORT, HOST);

console.log('Server listening on ' + HOST +':'+ PORT);
