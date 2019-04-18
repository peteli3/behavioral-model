#include "extern.h"
#include "named_p4object.h"

namespace bm {
  class ExternCounter : public ExternType {
   public:
    static constexpr unsigned int PACKETS = 0;
    static constexpr unsigned int BYTES = 1;

    BM_EXTERN_ATTRIBUTES {
      BM_EXTERN_ATTRIBUTE_ADD(init_count);
      BM_EXTERN_ATTRIBUTE_ADD(type);
    }

    void reset() {
      count = init_count_;
    }

    void init() override {
      init_count_ = init_count.get<size_t>();
      type_ = type.get<unsigned int>();
      reset();
    }

    void increment() {
      if (type_ == PACKETS) {
        count++;
      } else if (type_ == BYTES) {
        count += get_packet().get_ingress_length();
      } else {
        assert(0);
      }
    }

    void increment_by(const Data &d) {
      count += d.get<size_t>();
    }

    size_t get() const {
      return count;
    }

   private:
    // declared attributes
    Data init_count{0};
    Data type{PACKETS};

    // implementation members
    unsigned int type_{0};
    size_t init_count_{0};
    size_t count{0};
  };
}  // namespace bm
