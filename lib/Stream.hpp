#pragma once

#include <string>
#include <vector>

template<typename T>
class Stream : public std::vector<T> {
public:
    Stream() = default;
    Stream(const std::vector<T>& vector) : std::vector<T>(vector) { }
    Stream(const std::initializer_list<T>& list) : std::vector<T>(list) { }
    Stream(const T& begin, const T& end) : std::vector<T>(begin, end) { }

    Stream<T> filter(const std::function<bool(const T&)>& predicate) const {
        Stream<T> stream;

        for (const T& element : *this) {
            if (predicate(element)) {
                stream.push_back(element);
            }
        }

        return stream;
    }

    Stream<T> filter(const std::function<bool(const T&, const size_t)>& predicate) const {
        Stream<T> stream;

        for (size_t i = 0; i < this->size(); i++) {
            if (predicate(this->at(i), i)) {
                stream.push_back(this->at(i));
            }
        }

        return stream;
    }

    template<typename R = T>
    Stream<R> map(const std::function<R(const T&)>& mapper) const {
        Stream<R> stream;

        for (const T& element : *this) {
            stream.push_back(mapper(element));
        }

        return stream;
    }

    template<typename R = T>
    Stream<R> map(const std::function<R(const T&, const size_t)>& mapper) const {
        Stream<R> stream;

        for (size_t i = 0; i < this->size(); i++) {
            stream.push_back(mapper(this->at(i), i));
        }

        return stream;
    }

    template<typename R = T>
    R reduce(const std::function<R(const R&, const T&)>& reducer, const R& initial) const {
        R accumulator = initial;

        for (const T& element : *this) {
            accumulator = reducer(accumulator, element);
        }

        return accumulator;
    }

    template<typename R = T>
    R reduce(const std::function<R(const R&, const T&, const size_t)>& reducer, const R& initial) const {
        R accumulator = initial;

        for (size_t i = 0; i < this->size(); i++) {
            accumulator = reducer(accumulator, this->at(i), i);
        }

        return accumulator;
    }

    Stream<T> sort(const std::function<bool(const T&, const T&)>& comparator = [](const T& a, const T& b) { a < b; }) const {
        Stream<T> stream = *this;

        std::sort(stream.begin(), stream.end(), comparator);

        return stream;
    }

    Stream<T> concat(const Stream<T>& stream) const {
        Stream<T> result = *this;

        for (const T& element : stream) {
            result.push_back(element);
        }

        return result;
    }

    Stream<T> concat(const std::vector<T>& vector) const {
        return this->concat(Stream<T>(vector));
    }

    Stream<T> concat(const std::initializer_list<T>& list) const {
        return this->concat(Stream<T>(list));
    }

    Stream<T> slice(const size_t start, const size_t end) const {
        Stream<T> stream;

        for (size_t i = start; i < end; i++) {
            stream.push_back(this->at(i));
        }

        return stream;
    }

    Stream<T> slice(const size_t start) const {
        return this->slice(start, this->size());
    }

    Stream<T> splice(const size_t start, const size_t count, const Stream<T>& replacement = {}) const {
        Stream<T> stream = *this;

        stream.erase(stream.begin() + start, stream.begin() + start + count);

        if (replacement.size()) {
            stream.insert(stream.begin() + start, replacement.begin(), replacement.end());
        }

        return stream;
    }

    template<typename R>
    Stream<R> cast() const {
        return this->map<R>([](const T& element) { return reinterpret_cast<R>(element); });
    }

    Stream<T> unique() const {
        Stream<T> stream;

        for (const T& element : *this) {
            if (std::find(stream.begin(), stream.end(), element) == stream.end()) {
                stream.push_back(element);
            }
        }

        return stream;
    }

    Stream<T> reverse() const {
        Stream<T> stream = *this;

        std::reverse(stream.begin(), stream.end());

        return stream;
    }

    void forEach(const std::function<void(const T&)>& consumer) const {
        for (const T& element : *this) {
            consumer(element);
        }
    }

    void forEach(const std::function<void(const T&, const size_t)>& consumer) const {
        for (size_t i = 0; i < this->size(); i++) {
            consumer(this->at(i), i);
        }
    }
};

class StringStreamer {
public:
    static Stream<std::string> of(const std::string& input, const char delimiter = '\n') {
        std::stringstream stream(input);
        Stream<std::string> result;

        for (std::string line; std::getline(stream, line, delimiter);) {
            result.push_back(line);
        }

        return result;
    }
};