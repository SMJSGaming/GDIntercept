#pragma once

#include <string>
#include <vector>

template<typename T>
class Stream : public std::vector<T> {
    template <typename>
    struct is_stream : std::false_type {};

    template <typename U>
    struct is_stream<Stream<U>> : std::true_type {};
public:
    Stream() = default;
    Stream(const std::vector<T>& vector) : std::vector<T>(vector) { }
    Stream(const std::initializer_list<T>& list) : std::vector<T>(list) { }
    template <typename I>
    Stream(const I& begin, const I& end) : std::vector<T>(begin, end) { }
    template <typename ...Args> requires(std::convertible_to<Args, T> && ...)
    Stream(Args&& ...args) : std::vector<T>{ std::forward<Args>(args)... } { }

    template<std::invocable<const T&> F>
    Stream<T> filter(F&& predicate) const {
        Stream<T> stream;

        for (const T& element : *this) {
            if (predicate(element)) {
                stream.push_back(element);
            }
        }

        return stream;
    }

    template<std::invocable<const T&, const size_t> F>
    Stream<T> filter(F&& predicate) const {
        Stream<T> stream;

        for (size_t i = 0; i < this->size(); i++) {
            if (predicate(this->at(i), i)) {
                stream.push_back(this->at(i));
            }
        }

        return stream;
    }

    template<std::invocable<const T&> F>
    bool every(F&& predicate) const {
        for (const T& element : *this) {
            if (!predicate(element)) {
                return false;
            }
        }

        return true;
    }

    template<std::invocable<const T&, const size_t> F>
    bool every(F&& predicate) const {
        for (size_t i = 0; i < this->size(); i++) {
            if (!predicate(this->at(i), i)) {
                return false;
            }
        }

        return true;
    }

    template<std::invocable<const T&> F>
    bool some(F&& predicate) const {
        for (const T& element : *this) {
            if (predicate(element)) {
                return true;
            }
        }

        return false;
    }

    template<std::invocable<const T&, const size_t> F>
    bool some(F&& predicate) const {
        for (size_t i = 0; i < this->size(); i++) {
            if (predicate(this->at(i), i)) {
                return true;
            }
        }

        return false;
    }

    template<typename R>
    Stream<R> map(auto&& mapper) const requires(std::invocable<decltype(mapper), const T&>) {
        Stream<R> stream;

        for (const T& element : *this) {
            stream.push_back(mapper(element));
        }

        return stream;
    }

    template<typename R>
    Stream<R> map(auto&& mapper) const requires(std::invocable<decltype(mapper), const T&, const size_t>) { 
        Stream<R> stream;

        for (size_t i = 0; i < this->size(); i++) {
            stream.push_back(mapper(this->at(i), i));
        }

        return stream;
    }

    template<typename R>
    R reduce(auto&& reducer, const R& initial) const requires(std::invocable<decltype(reducer), const R&, const T&>) {
        R accumulator = initial;

        for (const T& element : *this) {
            accumulator = reducer(accumulator, element);
        }

        return accumulator;
    }

    template<typename R>
    R reduce(auto&& reducer, const R& initial) const requires(std::invocable<decltype(reducer), const R&, const T&, const size_t>) {
        R accumulator = initial;

        for (size_t i = 0; i < this->size(); i++) {
            accumulator = reducer(accumulator, this->at(i), i);
        }

        return accumulator;
    }

    template<std::invocable<const T&, const T&> F>
    Stream<T> sort(F&& comparator = [](const T& a, const T& b) { a < b; }) const {
        Stream<T> stream = *this;

        std::sort(stream.begin(), stream.end(), comparator);

        return stream;
    }

    template<std::invocable<const T&> F>
    Stream<T> forEach(F&& consumer) const {
        for (const T& element : *this) {
            consumer(element);
        }

        return *this;
    }

    template<std::invocable<const T&, const size_t> F>
    Stream<T> forEach(F&& consumer) const {
        for (size_t i = 0; i < this->size(); i++) {
            consumer(this->at(i), i);
        }

        return *this;
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

    auto flat() const requires(is_stream<T>::value) {
        Stream<typename T::value_type> stream;

        for (const T& element : *this) {
            stream.concat(element);
        }

        return stream;
    }

    Stream<T> reverse() const {
        Stream<T> stream = *this;

        std::reverse(stream.begin(), stream.end());

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
        return this->map<R>([](const T& element) { return static_cast<R>(element); });
    }

    template<std::invocable F>
    Stream<T> orExecute(F&& execution) const {
        if (this->empty()) {
            execution();
        }

        return *this;
    }

    bool includes(const T& value) const {
        return std::find(this->begin(), this->end(), value) != this->end();
    }

    bool includes(const std::vector<T>::iterator& iterator) const {
        return iterator != this->end();
    }

    auto getIterator(const T& value) const {
        return std::find(this->begin(), this->end(), value);
    }
};

class IntStream {
public:
    template<typename T = long long> requires(std::is_arithmetic_v<T>)
    static Stream<T> range(const T exclusiveEnd) {
        return IntStream::range<T>(0, exclusiveEnd);
    }

    template<typename T = long long> requires(std::is_arithmetic_v<T>)
    static Stream<T> range(const T includeStart, const T exclusiveEnd) {
        Stream<T> stream;

        for (T i = includeStart; i < exclusiveEnd; i++) {
            stream.push_back(i);
        }

        return stream;
    }
};

class StringStream {
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

template <typename I>
Stream(const I&, const I&) -> Stream<typename std::iterator_traits<I>::value_type>;