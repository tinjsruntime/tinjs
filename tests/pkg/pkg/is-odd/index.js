const _isEven = require('@is-even')

module.exports.default = function isOdd(n) {
    return !_isEven(n);
}