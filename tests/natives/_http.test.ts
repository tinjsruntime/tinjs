(function () {
    const { expect, describe, test } = require('tin:test');

    describe('http', () => {
        test('GET', () => {
            const { response } = fetch('http://localhost:3099/');

            expect(response).toEqual('Hello World from TIN! sent by client');
        });

        test('GET /html', () => {
            const { response } = fetch('http://localhost:3099/html');

            expect(response).toEqual('<h1>Hello World from TIN! sent by client</h1>');
        });

        test('GET /json', () => {
            const { response } = fetch('http://localhost:3099/json');

            expect(response).toEqual('{"message":"Hello World from TIN! sent by client"}');
        });

        test('POST /post', () => {
            const { response } = fetch('http://localhost:3099/post', {
                method: 'POST',
                body: JSON.stringify('{"message":"hey"}'),
                headers: {
                    'Content-type': 'application/json; charset=UTF-8',
                },
            });

            expect(response).toEqual('{"message":"yo"}');
        });
    });

    println("Tests for GET & POST ran, but currently tin's fetch does not support PUT, DELETE, PATCH methods,")
    println("if you want to test them, feel free to use curl or postman.");
})();