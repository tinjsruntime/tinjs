const { expect, describe, test } = require('tin:test');

describe('a', () => {
    test('a', () => {
        const x: number = 5;
        expect(x).toEqual(5);
    });
});