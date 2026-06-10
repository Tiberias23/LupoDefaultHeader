//
// Created by lupo on 10.06.26.
//

#ifndef LUPO_DEFAULT_HEADER_ERRORS_AS_VALUES_H
#define LUPO_DEFAULT_HEADER_ERRORS_AS_VALUES_H

#include <string>
#include <variant>


//----------------------------------------------------------------------------------------------------------------------
// Hier wird eine art Error as Value klasse implementiert ist etwas lang (und etwas hässlich), aber is praktisch
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Marker-Typ für erfolgreiche `Result`-Werte ohne Nutzdaten.
 *
 * @details
 * Wird als Standard-`OkType` verwendet, wenn nur "Erfolg/Fehler" signalisiert
 * werden soll, aber kein konkreter Erfolgswert benötigt wird.
 */
struct OkTag {};

/**
 * @brief Wrapper für den Erfolgswert (`Ok`) innerhalb von `Result`.
 *
 * @tparam T Typ des gespeicherten Erfolgswerts.
 */
template <typename T>
struct OkContainer {
    T value; ///< Enthaltener Erfolgswert.

    /**
     * @brief Konstruiert den Container per Move.
     * @param value Zu übernehmender Wert.
     */
    constexpr OkContainer(T&& value) : value(std::move(value)) {}

    /**
     * @brief Konstruiert den Container per Copy.
     * @param value Zu kopierender Wert.
     */
    constexpr OkContainer(const T& value) : value(value) {}
};

/**
 * @brief Wrapper für den Fehlerwert (`Err`) innerhalb von `Result`.
 *
 * @tparam T Typ des gespeicherten Fehlerwerts.
 */
template <typename T>
struct ErrContainer {
    T value; ///< Enthaltener Fehlerwert.

    /**
     * @brief Konstruiert den Container per Move.
     * @param value Zu übernehmender Wert.
     */
    constexpr ErrContainer(T&& value) : value(std::move(value)) {}

    /**
     * @brief Konstruiert den Container per Copy.
     * @param value Zu kopierender Wert.
     */
    constexpr ErrContainer(const T& value) : value(value) {}
};

/**
 * @brief Einfacher Result-Typ nach dem Muster "Ok oder Err".
 *
 * @tparam OkType Typ des Erfolgswerts.
 * @tparam ErrType Typ des Fehlerwerts.
 *
 * @details
 * Intern wird ein `std::variant<OkContainer<OkType>, ErrContainer<ErrType>>`
 * verwendet. Über `isOk()`/`isErr()` kann der aktive Zustand abgefragt werden.
 */
template <typename OkType = OkTag, typename ErrType = std::string>
class [[nodiscard]] Result {
public:
    /**
     * @brief Erzeugt ein `Result` im Ok-Zustand (Move).
     */
    constexpr Result(OkContainer<OkType>&& value) : value(std::move(value)) {}

    /**
     * @brief Erzeugt ein `Result` im Ok-Zustand (Copy).
     */
    constexpr Result(const OkContainer<OkType>& value) : value(value) {}

    /**
     * @brief Erzeugt ein `Result` im Err-Zustand (Move).
     */
    constexpr Result(ErrContainer<ErrType>&& value) : value(std::move(value)) {}

    /**
     * @brief Erzeugt ein `Result` im Err-Zustand (Copy).
     */
    constexpr Result(const ErrContainer<ErrType>& value) : value(value) {}

    /**
     * @brief Gibt den Erfolgswert zurück (rvalue-Objekt).
     * @return Referenz auf den Ok-Wert.
     * @warning Nur aufrufen, wenn `isOk()` wahr ist.
     */
    constexpr OkType& unwrap() && { return std::get<OkContainer<OkType>>(value).value; }

    /**
     * @brief Gibt den Erfolgswert zurück (const lvalue-Objekt).
     * @return Konstante Referenz auf den Ok-Wert.
     * @warning Nur aufrufen, wenn `isOk()` wahr ist.
     */
    [[nodiscard]] constexpr const OkType& unwrap() const& { return std::get<OkContainer<OkType>>(value).value; }

    /**
     * @brief Gibt den Fehlerwert zurück (rvalue-Objekt).
     * @return Referenz auf den Err-Wert.
     * @warning Nur aufrufen, wenn `isErr()` wahr ist.
     */
    constexpr ErrType& unwrapErr() && { return std::get<ErrContainer<ErrType>>(value).value; }

    /**
     * @brief Gibt den Fehlerwert zurück (const lvalue-Objekt).
     * @return Konstante Referenz auf den Err-Wert.
     * @warning Nur aufrufen, wenn `isErr()` wahr ist.
     */
    [[nodiscard]] constexpr const ErrType& unwrapErr() const& { return std::get<ErrContainer<ErrType>>(value).value; }

    /**
     * @brief Prüft, ob das Ergebnis ein Erfolgswert ist.
     * @retval true  Aktiver Zustand ist Ok.
     * @retval false Aktiver Zustand ist Err.
     */
    [[nodiscard]] constexpr bool isOk()  const { return std::holds_alternative<OkContainer<OkType>>(value); }

    /**
     * @brief Prüft, ob das Ergebnis ein Fehlerwert ist.
     * @retval true  Aktiver Zustand ist Err.
     * @retval false Aktiver Zustand ist Ok.
     */
    [[nodiscard]] constexpr bool isErr() const { return std::holds_alternative<ErrContainer<ErrType>>(value); }

    /**
     * @brief Liefert den Ok-Wert oder einen Defaultwert.
     * @param def Rückgabewert, falls der Zustand nicht Ok ist.
     * @return Referenz auf Ok-Wert oder `def`.
     */
    [[nodiscard]] constexpr const OkType&  unwrapOr   (const OkType&  def) const { return isOk()  ? unwrap()    : def; }

    /**
     * @brief Liefert den Err-Wert oder einen Defaultwert.
     * @param def Rückgabewert, falls der Zustand nicht Err ist.
     * @return Referenz auf Err-Wert oder `def`.
     */
    [[nodiscard]] constexpr const ErrType& unwrapErrOr(const ErrType& def) const { return isErr() ? unwrapErr() : def; }

private:
    /// Interne Speicherung des Zustands und des zugehörigen Werts.
    std::variant<OkContainer<OkType>, ErrContainer<ErrType>> value;
};

/**
 * @brief Hilfsfunktion zum Erzeugen eines Ok-`Result` aus einem rvalue.
 *
 * @tparam O Erfolgswert-Typ.
 * @tparam E Fehlerwert-Typ.
 * @param value Erfolgswert.
 * @return `Result<O, E>` im Ok-Zustand.
 */
template <typename O = OkTag, typename E = std::string>
constexpr Result<O, E> Ok(O&& value) { return Result<O, E>(OkContainer<O>(std::forward<O>(value))); }

/**
 * @brief Hilfsfunktion zum Erzeugen eines Ok-`Result` aus einem const lvalue.
 *
 * @tparam O Erfolgswert-Typ.
 * @tparam E Fehlerwert-Typ.
 * @param value Erfolgswert.
 * @return `Result<O, E>` im Ok-Zustand.
 */
template <typename O = OkTag, typename E = std::string>
constexpr Result<O, E> Ok(const O& value) { return Ok<O, E>(O(value)); }

/**
 * @brief Spezialisierte Ok-Hilfsfunktion für String-Literale.
 *
 * @tparam E Fehlerwert-Typ.
 * @tparam S Länge des String-Literals.
 * @param value String-Literal.
 * @return `Result<std::string, E>` im Ok-Zustand.
 */
template <typename E = std::string, size_t S>
constexpr Result<std::string, E> Ok(const char (&value)[S]) { return Ok<std::string, E>(std::string(value)); }

/**
 * @brief Erzeugt ein Ok-`Result` mit default-konstruiertem Erfolgswert.
 *
 * @tparam O Erfolgswert-Typ.
 * @tparam E Fehlerwert-Typ.
 * @return `Result<O, E>` im Ok-Zustand.
 */
template <typename O = OkTag, typename E = std::string>
constexpr Result<O, E> Ok() { return Ok<O, E>(O()); }

/**
 * @brief Hilfsfunktion zum Erzeugen eines Err-`Result` aus einem rvalue.
 *
 * @tparam O Erfolgswert-Typ.
 * @tparam E Fehlerwert-Typ.
 * @param value Fehlerwert.
 * @return `Result<O, E>` im Err-Zustand.
 */
template <typename O = OkTag, typename E = std::string>
constexpr Result<O, E> Err(E&& value) { return Result<O, E>(ErrContainer<E>(std::forward<E>(value))); }

/**
 * @brief Hilfsfunktion zum Erzeugen eines Err-`Result` aus einem const lvalue.
 *
 * @tparam O Erfolgswert-Typ.
 * @tparam E Fehlerwert-Typ.
 * @param value Fehlerwert.
 * @return `Result<O, E>` im Err-Zustand.
 */
template <typename O = OkTag, typename E = std::string>
constexpr Result<O, E> Err(const E& value) { return Err<O, E>(value); }

/**
 * @brief Spezialisierte Err-Hilfsfunktion für String-Literale.
 *
 * @tparam O Erfolgswert-Typ.
 * @tparam S Länge des String-Literals.
 * @param value String-Literal.
 * @return `Result<O, std::string>` im Err-Zustand.
 */
template <typename O = OkTag, size_t S>
constexpr Result<O, std::string> Err(const char (&value)[S]) { return Err<O, std::string>(std::string(value)); }


#endif //LUPO_DEFAULT_HEADER_ERRORS_AS_VALUES_H
