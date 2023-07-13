(function () {
    const { describe, test } = require('tin:test');

    describe('should show warning', () => {
        test('if callback throws', () => {
            throw new Error('This is expected to fail');
        });
    });
})();