// todo: if it's isEven, it throws: Error: SyntaxError: Can't create duplicate variable: 'isEven'
const _isEven = require('@is-even');

exports.default = function isOdd(n) {
    return !_isEven(n);
}