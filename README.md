my own implemantation of memory allocator like malloc.

## Malloc1
### 前提
* 先頭から指定された領域のメモリを返すだけ
* memory内にmetadata情報は入れない
### 結果
* freeのタイミングで、先頭アドレスが渡されるのはいいんだけど、どの大きさをfreeすればいいのかが謎という問題に直面した
* つまり、memoryに何らかのmetadataを保持しておく必要がありそう.


## Malloc2
* 前回の反省を踏まえて、freeする時に大きさがわかる構造としてメモリを管理する.
```

```

## mallocの仕様について
