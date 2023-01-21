template <typename T>
struct Signal {

};

template <typename T>
struct SignalObject {

};

template <typename T>
struct SignalSource: public SignalObject {

};

template <typename T>
struct SignalSink: public SignalObject {

};
