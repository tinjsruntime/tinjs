const d = fetch('https://dummyjson.com/products/1');
println(d.status)
println(d.json())
println(1)
println("x")
println(true);
println(function () {});
println([])
println([
    1,
    2,
    true,
    "x",
    function () {},
    [],
    {
        a: 1,
        b: 2,
        c: [],
        d: {},
        e: function () {},
        c: [
            1,
        ]
    },
    [1]
])

class A {}

println(A)
println(class {});
