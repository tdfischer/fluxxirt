class InputEvent {
public:
    enum Source {
      None,
      Acceleration,
    };

    InputEvent(Source s, int v)
      : m_source(s), m_value(v) {}

    InputEvent()
      : m_source(None), m_value(0) {}

    int value() const {
        return m_value;
    }

    Source source() const {
      return m_source;
    }

private:
    Source m_source;
    int m_value;
};

class Input {
public:
    Input();
    InputEvent read();
private:
    int m_lastGravity;
};
