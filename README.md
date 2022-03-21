my own implemantation of memory allocator,  like `malloc`.

## install
```
$ make install
```
To run this command, `cmake` required.
By default, this library compiled to shared obj and installed in `/usr/lib`.

## Example
I confirmed these operations on ubuntu 20.04.
```
$ git clone https://github.com/mox692/mmc
$ cd mmc && make install
$ cd examples && make build
$ ./main
```
In the example directory, there is a simple program which contains 
malloc() and free() operations and shows the changes in memory state.

Result will be like this,

```
== mem usage     ==
|addr 0000-0999 ... free(header: 0000-0039; data: 0040-0999) |

== mem usage end ==
== mem usage     ==
|addr 0000-0049 ... used(header: 0000-0039; data: 0040-0049) |
|addr 0050-0999 ... free(header: 0050-0089; data: 0090-0999) |

== mem usage end ==
== mem usage     ==
|addr 0000-0049 ... used(header: 0000-0039; data: 0040-0049) |
|addr 0050-0099 ... used(header: 0050-0089; data: 0090-0099) |
|addr 0100-0999 ... free(header: 0100-0139; data: 0140-0999) |

== mem usage end ==
== mem usage     ==
|addr 0000-0049 ... free(header: 0000-0039; data: 0040-0049) |
|addr 0050-0099 ... used(header: 0050-0089; data: 0090-0099) |
|addr 0100-0999 ... free(header: 0100-0139; data: 0140-0999) |

== mem usage end ==
== mem usage     ==
|addr 0000-0999 ... free(header: 0000-0039; data: 0040-0999) |

== mem usage end ==
```

## mallocの仕様について
todo...
