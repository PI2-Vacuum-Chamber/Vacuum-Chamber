const fs=require('fs');
var express = require('express');
var app = express();
const request = require('request');
var options = {
    hostname: 'localhost',
    port: app.get('8002'),
    path: '/target/newpoints',
    method: 'GET',
    json:true
}
request(options, function(error, response, data){
    if(error) console.log(error);
    else 
	fs.writeFile(newpoints.json, data);
});
