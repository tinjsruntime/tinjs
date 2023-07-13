(function () {
    const { expect, describe, test } = require('tin:test');

    describe('TypeScript types', () => {
        test('typed variable', () => {
            const x: number = 5;
            expect(x).toEqual(5);
        });
    });
})();