// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

namespace support
{
namespace cl
{

//--------------------------------------------------------------------------------------------------
// Parser
//

#ifdef QSTRING_H
template <>
struct Parser<QString>
{
    bool operator()(StringRef value, size_t /*i*/, QString& result) const
    {
        result.fromUtf8(value.data(), value.size());
        return true;
    }
};
#endif

#ifdef QURL_H
template <>
struct Parser<QUrl>
{
    bool operator()(StringRef value, size_t /*i*/, QUrl& result) const
    {
        result.setEncodedUrl(QByteArray(value.data(), value.size()), QUrl::StrictMode);
        return result.isValid();
    }
};
#endif

//--------------------------------------------------------------------------------------------------
// Traits
//

namespace qt
{
#ifdef QMAP_H
    struct QMapInserter
    {
        template <class C, class V>
        void operator()(C& c, V&& v) const {
            c.insert(std::forward<V>(v).first, std::forward<V>(v).second);
        }
    };
#endif

#ifdef QSET_H
    struct QSetInserter
    {
        template <class C, class V>
        void operator()(C& c, V&& v) const {
            c.insert(std::forward<V>(v));
        }
    };
#endif
}

#ifdef QHASH_H
template <class T, class U>
struct Traits<QHash<T, U>> : TraitsBase<std::pair<T, U>, qt::QMapInserter>
{
};

template <class T, class U>
struct Traits<QMultiHash<T, U>> : TraitsBase<std::pair<T, U>, qt::QMapInserter>
{
};
#endif

#ifdef QMAP_H
template <class T, class U>
struct Traits<QMap<T, U>> : TraitsBase<std::pair<T, U>, qt::QMapInserter>
{
};

template <class T, class U>
struct Traits<QMultiMap<T, U>> : TraitsBase<std::pair<T, U>, qt::QMapInserter>
{
};
#endif

#ifdef QSET_H
template <class T>
struct Traits<QSet<T>> : TraitsBase<typename QSet<T>::value_type, qt::QSetInserter>
{
};
#endif

#ifdef QSTRING_H
template <>
struct Traits<QString> : TraitsBase<QString, void>
{
};
#endif

} // namespace cl
} // namespace support