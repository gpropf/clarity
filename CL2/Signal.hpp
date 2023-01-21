

namespace cl2 {

template <typename T>
struct Signal {};

template <typename T>
struct SignalObject {
    T const * obj;
};

template <typename T>
struct SignalSource : public SignalObject<T> {};

template <typename T>
struct SignalSink : public SignalObject<T> {};

}  // namespace cl2