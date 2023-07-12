const { expect, describe, test } = require('tin:test');

function trim(string) {
  return string.replace(/\s/g, '');
}

describe('fetch', () => {
  test('GET', () => {
    const response = fetch('https://jsonplaceholder.typicode.com/posts/1');
    const data = response.json();
    
    expect(data.userId).toEqual(1);
    expect(data.id).toEqual(1);
  });

  test('POST', () => {
    const data = {
      title: 'foo',
      body: 'bar',
      userId: 1,
    };

    const response = fetch('https://jsonplaceholder.typicode.com/posts', {
      method: 'POST',
      body: JSON.stringify(data),
      headers: {
        'Content-type': 'application/json; charset=UTF-8',
      },
    });

    expect(response.status).toEqual(201);
    expect(trim(response.response)).toEqual(trim(JSON.stringify({...data, id: 101})));
  });
});