在Ecmascript(ES6)引入 TypedArray 之前，Javascript中没有用于处理二进制流的机制。Buffer 作为Node.js API 的一部分被引入，使得Javascript 与字节流交互成为可能。现在 TypedArray 被加入到 Es6 中， Buffer类是 Unit8Array （8字节无符号整数数列）API 在Node.js 应用场景中的实现
Buffer 类似于固定长度的整数数列，其内存由 V8 堆栈之外的原始内存分配。
Buffer 的大小在构造时确定， 一旦生成，其大小就不能再发生改变

```js
// Creates a zero-filled Buffer of length 10.
const buf1 = new Buffer.alloc(10);

// Creates a Buffer of length 10, filled with 0x1.
const buf2 = new Buffer.alloc(10,1);

// Creates an uninitialized buffer of length 10.
// This is faster than calling Buffer.alloc() but the returned
// Buffer instance might contain old data that needs to be
// overwritten using either fill() or write().
const buf3 = Buffer.allocUnsafe(10);

// Creates a Buffer containing [0x1, 0x2, 0x3].
const buf4 = Buffer.from([1,2,3]);

// Creates a Buffer containing UTF-8 bytes [0x74, 0xc3, 0xa9, 0x73, 0x74].

const buf5 = Buffer.from('test'); //默认为 utf-8 编码

// Creates a Buffer containing Latin-1 bytes [0x74, 0xe9, 0x73, 0x74].
const buf6 = Buffer.from('test','latin1');

```

相较之前版本中的通过 Buffer 构造函数，来创建 Buffer 的方式，现在通过 Buffer.from(), Buffer.alloc(), Buffer.allocUnsafe() 来构造,更加安全和稳定.

 - Buffer.from(arry) 返回包含原有字节副本的一个全新的 Buffer
 - Buffer.from(arryBuffer[,byteOffset[, length]]) 返回一个与原 arryBuffer 共享存储的新 Buffer
 - Buffer.from(buffer) 返回一个包含原有buffer 内容副本的新 buffer
 - Buffer.from(string [, encoding]) 返回包含原有字符串副本的新 Buffer (默认字符编码为 utf-8)
 - Buffer.alloc(size, [,fill [, encoding]]) 返回一个被填充好的固定具有确定长度的 Buffer 实例。这个方法可能会比 Buffer.allocUnsafe(size) 慢很多，但可以确保新建的 Buffer 中没有老旧的潜在敏感数据(更加安全)
 - Buffer.allocUnsafe(size) 和 Buffer.allocUnsafeSlow(size)， 各自返回一个确定长度的 Buffer，它必须使用 buff.fill(0) 或者逐个手工填入初始化数据

  如果 size 小于等于Buffer.poolSize的一半，Buffer.allocUnsafe(size)会从共享内存中分配空间。 而 Buffer.allocUnsafeSlow(size) 则不会

  Node.js 可以是 *--zero-fill-buffers* 命令行选项来强制所有新建 Buffer 实例自动用 **0** 填充。 这个命令行选项不仅会改变方法的默认行为，也会对性能产生重要影响。 仅当有必要强制新创建的 Buffer 不包含敏感信息时使用 \
  Example:
  ```dos
  $node --zero-fill-buffers
  > Buffer.allocUnsafe(5);
  <Buffer 00 00 00 00 00>
  ```

  是什么原因导致 Buffer.allocUnsafe(size) 和 Buffer.allocUnsafeSlow(size) 这两个方法是不安全的？\
  当我们调用者两个方法是，被分配出来的内存空间并会做初始化，这样的设计使得内存分配的动作非常快，但是被分配出来的内存段中可能包含敏感的老数据。 如果使用 Buffer.allocUnsafe()，而没有对内存做好覆盖(overwritting)，当我们对 Buffer 的内存块做read操作时，可能会导致老数据泄露。\
  虽然，使用 Buffer.allocUnsafe(size) 方法中效率上有很大优势，但需要额外注意避免因此而导致应用程序出现安全漏洞

  Buffer 实例常用来呈现一系列的编码后的字符。这使得 Buffer 与 普通 Javascript 字符串之间通过显式字符串编码相互转换成为可能。\
  Example
  ```javascript
  const buf = Buffer.from('hello world','acii');
  
  // Prints: 68656c6c6f20776f726c64
  console.log(buf.toString('hex'));
  
  // Prints: aGVsbG8gd29ybGQ=
  console.log(buf.toString('base64'));

  ```
  Node.js 支持的字符编码：
  - 'ascii' - for 7-bit ASCII data only. This encoding is fast and will strip the high bit if set.

  - 'utf8' - Multibyte encoded Unicode characters. Many web pages and other document formats use UTF-8.

  - 'utf16le' - 2 or 4 bytes, little-endian encoded Unicode characters. Surrogate pairs (U+10000 to U+10FFFF) are supported.

  - 'ucs2' - Alias of 'utf16le'.

  - 'base64' - Base64 encoding. When creating a Buffer from a string, this encoding will also correctly accept "URL and Filename Safe Alphabet" as specified in RFC4648, Section 5.

  - 'latin1' - A way of encoding the Buffer into a one-byte encoded string (as defined by the IANA in RFC1345, page 63, to be the Latin-1 supplement block and C0/C1 control codes).

  - 'binary' - Alias for 'latin1'.

  - 'hex' - Encode each byte as two hexadecimal characters.

  *Note*: Today's browsers follow the **WHATWG** spec which aliases both *'latin1'* and ISO-8859-1 to win-1252. This means that while doing something like **http.get()**, if the returned charset is one of those listed in the **WHATWG** spec it's possible that the server actually returned win-1252-encoded data, and using 'latin1' encoding may incorrectly decode the characters.

  Buffer 的实例对象也是 TypeArray 的实例对象，但是 Buffer 对象和 Es2015 中的 TypedArray 对象也有所不同。 例如 ArrayBuffer#slice() 会创建一个 slice的副本，但是 Buffer#slice()则会创建一个 slice 的视图，这样使得 Buffer#slice() 更加高效

  基于以下原因，使得从 Buffer 对象创建一个 TypedArray 对象成为可能:
  - Buffer 对象的内存是 TypedArray 对象的副本，而不是共享内存
  - Buffer 对象的内存，被解释为不同元素的数组，而非字节组。例如，new Unit32Array(Buffer.from([1,2,3,4])) 通过[1,2,3,4] 创建了一个含有 4 个元素的 Unit32Array，而不是一个只含有单个元素的 Unit32Array 对象

  可以通过 TypedArray 对象的 *.buffer* 属性，创建一个与 TypedArrary 实例共享内存的 Buffer \
  Example
  ```javascript
    const arr = new Unit16Array(2);
    arr[0] = 5000;
    arr[1] = 4000;

    // Copies the contents of `arr`
    const buf1 = Buffer.from(arr);

    // Shares memory with `arr`
    const buf2 = Buffer.from(arr.buffer);

    // Prints: <Buffer 88 a0>
    console.log(buf1);

    // Prints: <Buffer 88 13 a0 0f>
    console.log(buf2);

    arr[1] = 6000;

    // Prints: <Buffer 88 a0>
    console.log(buf1);

    // Prints: <Buffer 88 13 70 17>
    console.log(buf2);
  ```
  
  当使用 TypedArray 的 .buffer,创建一个新的 Buffer 对象时，可以通过传入 byteOffset 和 length，来强调使用 TypedArray 的哪一部分来创建\
  Example
  ```javascript
    const arr = new Unit16Array(20);
    const buf = Buffer.from(arr.buffer,0,16);

    // Prints: 16
    console.log(buf.length);
  ```

  Buffer 和 TypedArray 有不同的语法和实现。特别是，TypedArray 变体可以接受一个 mapping function 作为第二个参数，作用于这个数组的每个元素，例如：TypedArray.from(source [,mapFn [,thisArg]]); 但 Buffer.from() 方法则不行

  Buffer 实例可以通过 ECMAScript2015(Es6) 的 *for...of* 语法进行迭代\
  Example
  ```javascript
    const buf = Buffer.form([1,2,3]);
    
    // Prints:
    //   1
    //   2
    //   3
    for(const b of buf){
      console.log(b);
    }
  ```
  此外，可以通过 buf.values(), buf.keys(), buf.entries() 方法创建迭代器



  


  
  


