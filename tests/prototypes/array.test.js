const { expect, describe, test } = require('tin:test');

const arr = ["Hello", "Hola", "Hallo"];

describe('Array', () => {
    test('random()', () => {
        const random = arr.random();
        expect(arr.includes(random)).toEqual(true);
    });

    test('first()', () => {
        expect(arr.first()).toEqual(arr[0]);
    });

    test('last()', () => {
        expect(arr.last()).toEqual(arr[arr.length - 1]);
    });
});