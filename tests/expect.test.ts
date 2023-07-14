(function () {
    const { expect, describe, test } = require('tin:test');

    describe('expect()', () => {
        test('toBeBool() | toBeBoolean()', () => {
            expect(true).toBeBool();
            expect(false).toBeBool();
            expect(true).toBeBoolean();
            expect(false).toBeBoolean();
        });

        test('toBeTrue()', () => {
            expect(true).toBeTrue();
        });

        test('toEqual()', () => {
            expect('Hello World').toEqual('Hello World');
            expect(1).toEqual(1);
        });
    });
})();