(function () {
    const { expect, describe, test } = require('tin:test');

    const arr = ["Hello", "Hola", "Hallo"];

    describe('Array', () => {
        test('random()', () => {
            expect(arr.includes(arr.random())).toBeTrue();
        });

        test('first()', () => {
            expect(arr.first()).toEqual(arr[0]);
        });

        test('last()', () => {
            expect(arr.last()).toEqual(arr[arr.length - 1]);
        });
    });

    const emptyArr = [];

    describe('Array - empty', () => {
        test('random()', () => {
            expect(emptyArr.random()).toEqual(undefined);
        });

        test('first()', () => {
            expect(emptyArr.first()).toEqual(undefined);
        });

        test('last()', () => {
            expect(emptyArr.last()).toEqual(undefined);
        });
    });
})();