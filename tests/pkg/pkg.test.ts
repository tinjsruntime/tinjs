(function () {
    const { expect, describe, test } = require('tin:test');
    
    const isOdd = require('@is-odd');
    const isEven = require('@is-even');

    describe('example', () => {
        test('isEven', () => {
            expect(isEven(1)).toBeFalse();
            expect(isEven(2)).toBeTrue();
        })

        test('isOdd - isEven as dep.', () => {
            expect(isOdd(1)).toBeTrue();
            expect(isOdd(2)).toBeFalse();
        });
    });
})();