const http = require('tin:http');
const server = http.createServer();

const port = 3099;
println('Server started at http://localhost:' + port);

server.get('/', (req) => {
    return { content: 'Hello World from TIN! sent by client', type: 'plain' };
});

server.get('/post', (req) => {
    return { content: "You should use POST you bad programmer!", type: 'plain'};
});

server.post('/post', (req) => {
    return { content: { message: 'yo' } , type: 'json' };
});

server.get('/html', (req) => {
    return { content: '<h1>Hello World from TIN! sent by client</h1>', type: 'html' };
});

// test put, delete and patch
server.put('/put', (req) => {
    return { content: { message: 'PUT' }, type: 'json' };
});

server.delete('/delete', (req) => {
    return { content: { message: 'DELETE' }, type: 'json' };
});

server.patch('/patch', (req) => {
    return { content: { message: 'PATCH' }, type: 'json' };
});

server.listen(port);