const http = require("http");
const host = 'localhost';
const port = 8001;
const fs = require('fs')
const requestListener = function (req, res) {
    res.setHeader("Content-Type", "application/json");
    switch (req.url) {
        case "/sensor/newpoints":
            res.writeHead(200);
            data = fs.readFileSync('newpoints.json', 'utf8');
            res.end(data);
            break
        default:
            res.writeHead(404);
	    res.end(JSON.stringify({error:"Não encontrado"}));
    }
}
const server = http.createServer(requestListener);
server.listen(port, host, () => {
    console.log(`Server is running on http://${host}:${port}`);
});

