## Implementation Progress

### C++23 Support
This table tracks implementation of C++23 features on a per-proposal basis.

| Proposal Link | Synopsis | Completed | Blocked | Notes |
| ------------- | -------- | --------- | ------- | ----- |


### C++20 Headers
The library started out as an C++20 standard library. This table tracks the progress of implementation of the C++20 headers.

| Header Name | Completed | Blocked | Implementing | Hasn't Started | Notes |
| ----------- | --------- | ------- | ------------ | -------------- | ----- |
| `concepts` | &check; | | | | |
| `coroutine` | &check; | | | | |
| `cstdlib` | &check; | | | | |
| `csignal` | &check; | | | | |
| `csetjmp` | &check; | | | | |
| `cstdarg` | &check; | | | | |
| `typeinfo` | &check; | | | | |
| `typeindex` | &check; | | | | |
| `type_traits` | &check; | | | | |
| `bitset` | | &check; | | | Blocked due to complicated bit-shifting logic. |
| `functional` | | | | &check; | |
| `utility` | &check; | | | | |
| `ctime` | &check; | | | | |
| `chrono` | | | | &check; | |
| `cstddef` | &check; | | | | |
| `initializer_list` | &check; | | | | |
| `tuple` | | &check; | | | Blocked due to unresolved circular dependency between `tuple` and `memory` for `uses-allocator` constructors. |
| `any` | &check; | | | | |
| `optional` | &check; | | | | |
| `variant` | | | | &check; | |
| `compare` | | &check; | | | Blocked due to complicated float comparison logic. |
| `version` | &check; | | | | |
| `source_location` | | &check; | | | Blocked due to lack of intrinsic support from GCC. |
| `new` | &check; | | | | |
| `memory` | | &check; | | | Blocked due to `iostream` not yet implemented, and that it's unclear how to implement `shared_ptr`'s unbounded array constructor, and also the memory algorithms. |
| `scoped_allocator` | &check; | | | | |
| `memory_resource` | | &check; | | | Blocked due to unimplemented `pool_resources`. |
| `climits` | &check; | | | | |
| `cfloat` | &check; | | | | |
| `cstdint` | &check; | | | | |
| `cinttypes` | &check; | | | | |
| `limits` | &check; | | | | |
| `exception` | &check; | | | | |
| `stdexcept` | &check; | | | | |
| `cassert` | &check; | | | | |
| `system_error` | &check; | | | | |
| `cerrno` | &check; | | | | |
| `cctype` | &check; | | | | |
| `cwctype` | &check; | | | | |
| `cstring` | &check; | | | | |
| `cwchar` | &check; | | | | |
| `cuchar` | | &check; | | | Blocked due to unknown logic for `mbrtoc8` and `c8rtomb`. |
| `string` |  |&check; | | | Blocked due to `iostream` not yet implemented. |
| `string_view` | &check; | | | | |
| `charconv` | | | | &check; | |
| `format` | | | | &check; | |
| `array` | &check; | | | | |
| `vector` | | | | &check; | |
| `deque` | | | | &check; | |
| `list` | | | | &check; | |
| `forward_list` | | | | &check; | |
| `set` | | | | &check; | |
| `map` | | | | &check; | |
| `unordered_set` | | | | &check; | |
| `unordered_map` | | | | &check; | |
| `stack` | | | | &check; | |
| `queue` | | | | &check; | |
| `span` | &check; | | | | |
| `iterator` | | | &check; | | |
| `ranges` | | | | &check; | |
| `algorithm` | | | | &check; | |
| `execution` | | | | &check; | |
| `cmath` | | | | &check; | |
| `complex` | | | | &check; | |
| `valarray` | | | | &check; | |
| `random` | | | | &check; | |
| `numeric` | | | | &check; | |
| `ratio` | | | | &check; | |
| `cfenv` | &check; | | | | |
| `bit` | &check; | | | | |
| `numbers` | &check; | | | | |
| `locale` | | | &check; |  | |
| `clocale` | &check; | | | | |
| `iosfwd` | &check; | | | | |
| `ios` | &check; | | | | |
| `istream` | | | | &check; | |
| `ostream` | | | | &check; | |
| `iostream` | | | | &check; | |
| `fstream` | | | | &check; | |
| `sstream` | | | | &check; | |
| `syncstream` | | | | &check; | |
| `iomanip` | | | | &check; | |
| `streambuf` | &check; | | | | |
| `cstdio` | &check; | | | | |
| `filesystem` | | | | &check; | |
| `regex` | | | | &check; | |
| `atomic` | | | | &check; | |
| `thread` | | &check; | | | Blocked due to unimplemented `iostream`. |
| `stop_token` | | &check; | | | Blocked due to possibly buggy `request_stop` implementation. |
| `mutex` | | &check; | | | Blocked due to the unimplemented `lock` algorithm. |
| `shared_mutex` | &check; | | | | |
| `future` | | | | &check; | |
| `condition_variable` | &check; | | | | |
| `semaphore` | &check; | | | | |
| `latch` | &check; | | | | |
| `barrier` | | | | &check; | |
