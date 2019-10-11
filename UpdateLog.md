Update 10.11

修改了加密的layer的定义方式,现在强弱key的加密layer层数在全局定义了.

修改了加密函数的调用流程.由于需要把所有的pixel放在空间上连续的一段进行加密,所以调用的顺序变成了dct_frame(), stream_encrypt(), idct_frame(), dct过程只把layer处的pixel放在明文数组中, idct过程只把密文数组放回pixel中.