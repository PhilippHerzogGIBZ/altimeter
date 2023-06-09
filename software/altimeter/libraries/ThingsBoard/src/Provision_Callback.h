/*
  Provision_Callback.h - Library API for sending data to the ThingsBoard
  Based on PubSub MQTT library.
  Created by Olender M. Oct 2018.
  Released into the public domain.
*/
#ifndef Provision_Callback_h
#define Provision_Callback_h

// Local includes.
#include "Configuration.h"

// Library includes.
#include <ArduinoJson.h>
#if THINGSBOARD_ENABLE_STL
#include <functional>
#endif // THINGSBOARD_ENABLE_STL

/// ---------------------------------
/// Constant strings in flash memory.
/// ---------------------------------
#if THINGSBOARD_ENABLE_PROGMEM
constexpr char PROVISION_CB_IS_NULL[] PROGMEM = "Provisioning callback is NULL";
constexpr char ACCESS_TOKEN_CRED_TYPE[] PROGMEM = "ACCESS_TOKEN";
constexpr char MQTT_BASIC_CRED_TYPE[] PROGMEM = "MQTT_BASIC";
constexpr char X509_CERTIFICATE_CRED_TYPE[] PROGMEM = "X509_CERTIFICATE";
#else
constexpr char PROVISION_CB_IS_NULL[] = "Provisioning callback is NULL";
constexpr char ACCESS_TOKEN_CRED_TYPE[] = "ACCESS_TOKEN";
constexpr char MQTT_BASIC_CRED_TYPE[] = "MQTT_BASIC";
constexpr char X509_CERTIFICATE_CRED_TYPE[] = "X509_CERTIFICATE";
#endif // THINGSBOARD_ENABLE_PROGMEM

// Convenient aliases
using Provision_Data = const JsonObjectConst;

// Struct dispatch tags, to differentiate between constructors, allows the same paramter types to be passed
struct Access_Token{};
struct Device_Access_Token{};
struct Basic_MQTT_Credentials{};
struct X509_Certificate{};

/// @brief Provisioning callback wrapper
class Provision_Callback {
  public:
    /// @brief Provisioning callback signature
    using returnType = void;
    using argumentType = const Provision_Data&;
#if THINGSBOARD_ENABLE_STL
    using processFn = std::function<returnType(argumentType data)>;
#else
    using processFn = returnType (*)(argumentType data);
#endif // THINGSBOARD_ENABLE_STL

    /// @brief Constructs empty callback, will result in never being called
    inline Provision_Callback()
      : Provision_Callback(Access_Token(), nullptr, nullptr, nullptr) {  }

    /// @brief Constructs callback that will be fired upon a provision request arrival,
    /// where the requested credentials were sent by the cloud and received by the client.
    /// Using the credentials generated by the ThingsBoard server method. See https://thingsboard.io/docs/user-guide/device-provisioning/?mqttprovisioning=without#mqtt-device-apis
    /// @param cb Callback method that will be called
    /// @param provisionDeviceKey Device profile provisioning key of the device profile that should be used to create the device under
    /// @param provisionDeviceSecret Device profile provisioning secret of the device profile that should be used to create the device under
    /// @param deviceName Name the created device should have on the cloud,
    /// pass nullptr or an empty string if the access token should be used as a name instead
    inline Provision_Callback(Access_Token, processFn cb, const char *provisionDeviceKey, const char *provisionDeviceSecret, const char *deviceName = nullptr)
      : m_cb(cb), m_deviceKey(provisionDeviceKey), m_deviceSecret(provisionDeviceSecret), m_deviceName(deviceName), m_accessToken(nullptr), m_credUsername(nullptr), m_credPassword(nullptr), m_credClientID(nullptr), m_hash(nullptr), m_credentialsType(nullptr)  {  }

    /// @brief Constructs callback that will be fired upon a provision request arrival,
    /// where the requested credentials were sent by the cloud and received by the client.
    /// Using the device supplies access token method. See https://thingsboard.io/docs/user-guide/device-provisioning/?mqttprovisioning=without#mqtt-device-apis
    /// @param cb Callback method that will be called
    /// @param provisionDeviceKey Device profile provisioning key of the device profile that should be used to create the device under
    /// @param provisionDeviceSecret Device profile provisioning secret of the device profile that should be used to create the device under
    /// @param accessToken Access token generated by the device, that will be used by the provisioned device, alternative to letting the access token be generated by the cloud instead
    /// @param deviceName Name the created device should have on the cloud,
    /// pass nullptr or an empty string if the access token should be used as a name instead
    inline Provision_Callback(Device_Access_Token, processFn cb, const char *provisionDeviceKey, const char *provisionDeviceSecret, const char *accessToken, const char *deviceName = nullptr)
      : m_cb(cb), m_deviceKey(provisionDeviceKey), m_deviceSecret(provisionDeviceSecret), m_deviceName(deviceName), m_accessToken(accessToken), m_credUsername(nullptr), m_credPassword(nullptr), m_credClientID(nullptr), m_hash(nullptr), m_credentialsType(ACCESS_TOKEN_CRED_TYPE)  {  }

    /// @brief Constructs callback that will be fired upon a provision request arrival,
    /// where the requested credentials were sent by the cloud and received by the client.
    /// Using the device supplies basic MQTT credentials method. See https://thingsboard.io/docs/user-guide/device-provisioning/?mqttprovisioning=without#mqtt-device-apis
    /// @param cb Callback method that will be called
    /// @param provisionDeviceKey Device profile provisioning key of the device profile that should be used to create the device under
    /// @param provisionDeviceSecret Device profile provisioning secret of the device profile that should be used to create the device under
    /// @param username Basic MQTT credentials username, that will be used by the provisioned device
    /// @param password Basic MQTT credentials password, that will be used by the provisioned device
    /// @param clientID Basic MQTT credentials clientID, that will be used by the provisioned device
    /// @param deviceName Name the created device should have on the cloud,
    /// pass nullptr or an empty string if the access token should be used as a name instead
    inline Provision_Callback(Basic_MQTT_Credentials, processFn cb, const char *provisionDeviceKey, const char *provisionDeviceSecret, const char *username, const char *password, const char *clientID, const char *deviceName = nullptr)
      : m_cb(cb), m_deviceKey(provisionDeviceKey), m_deviceSecret(provisionDeviceSecret), m_deviceName(deviceName), m_accessToken(nullptr), m_credUsername(username), m_credPassword(password), m_credClientID(clientID), m_hash(nullptr), m_credentialsType(MQTT_BASIC_CRED_TYPE)  {  }

    /// @brief Constructs callback that will be fired upon a provision request arrival,
    /// where the requested credentials were sent by the cloud and received by the client.
    /// Using the device supplies X.509 certificate method. See https://thingsboard.io/docs/user-guide/device-provisioning/?mqttprovisioning=without#mqtt-device-apis
    /// @param cb Callback method that will be called
    /// @param provisionDeviceKey Device profile provisioning key of the device profile that should be used to create the device under
    /// @param provisionDeviceSecret Device profile provisioning secret of the device profile that should be used to create the device under
    /// @param hash Public X.509 certificate hash, that will be used by the provisioned device
    /// @param deviceName Name the created device should have on the cloud,
    /// pass nullptr or an empty string if the access token should be used as a name instead
    inline Provision_Callback(X509_Certificate, processFn cb, const char *provisionDeviceKey, const char *provisionDeviceSecret, const char *hash, const char *deviceName = nullptr)
      : m_cb(cb), m_deviceKey(provisionDeviceKey), m_deviceSecret(provisionDeviceSecret), m_deviceName(deviceName), m_accessToken(nullptr), m_credUsername(nullptr), m_credPassword(nullptr), m_credClientID(nullptr), m_hash(hash), m_credentialsType(X509_CERTIFICATE_CRED_TYPE)  {  }

    /// @brief Calls the callback that was subscribed, when this class instance was initally created
    /// @tparam Logger Logging class that should be used to print messages
    /// @param data Received shared attribute request data that include
    /// the credentials that was requested
    template<typename Logger>
    inline returnType Call_Callback(argumentType data) const {
      // Check if the callback is a nullptr,
      // meaning it has not been assigned any valid callback method.
      if (!m_cb) {
        Logger::log(PROVISION_CB_IS_NULL);
        return returnType();
      }
      return m_cb(data);
    }

    /// @brief Sets the callback method that will be called
    /// @param cb Callback method that will be called
    inline void Set_Callback(processFn cb) {
      m_cb = cb;
    }

    /// @brief Gets the device profile provisioning key of the device profile,
    /// that should be used to create the device under
    /// @return Device profile provisioning key
    inline const char* Get_Device_Key() const {
      return m_deviceKey;
    }

    /// @brief Sets the device profile provisioning key of the device profile,
    /// that should be used to create the device under
    /// @param provisionDeviceKey Device profile provisioning key
    inline void Set_Device_Key(const char *provisionDeviceKey) {
      m_deviceKey = provisionDeviceKey;
    }

    /// @brief Gets the device profile provisioning secret of the device profile,
    /// that should be used to create the device under
    /// @return Device profile provisioning secret
    inline const char* Get_Device_Secret() const {
      return m_deviceSecret;
    }

    /// @brief Gets the device profile provisioning secret of the device profile,
    /// that should be used to create the device under
    /// @param provisionDeviceSecret Device profile provisioning secret
    inline void Set_Device_Secret(const char *provisionDeviceSecret) {
      m_deviceSecret = provisionDeviceSecret;
    }

    /// @brief Gets the name the created device should have on the cloud,
    /// is a nullptr or an empty string if the access token should be used as a name instead
    /// @return Name the created device should have on the cloud
    inline const char* Get_Device_Name() const {
      return m_deviceName;
    }

    /// @brief Sets the name the created device should have on the cloud,
    /// is a nullptr or an empty string if the access token should be used as a name instead
    /// @param deviceName Name the created device should have on the cloud
    inline void Set_Device_Name(const char *deviceName) {
      m_deviceName = deviceName;
    }

    /// @brief Gets the access token generated by the device,
    /// that will be used by the provisioned device,
    /// alternative to letting the access token be generated by the cloud instead
    /// @return Access token generated by the device
    inline const char* Get_Device_Access_Token() const {
      return m_accessToken;
    }

    /// @brief Sets the access token generated by the device,
    /// that will be used by the provisioned device,
    /// alternative to letting the access token be generated by the cloud instead
    /// @param accessToken Access token generated by the device
    inline void Set_Device_Access_Token(const char *accessToken) {
      m_accessToken = accessToken;
    }

    /// @brief Gets the basic MQTT credentials username, that will be used by the provisioned device
    /// @return Basic MQTT credentials username
    inline const char* Get_Credentials_Username() const {
      return m_credUsername;
    }

    /// @brief Sets the basic MQTT credentials username, that will be used by the provisioned device
    /// @param username Basic MQTT credentials username
    inline void Set_Credentials_Username(const char *username) {
      m_credUsername = username;
    }

    /// @brief Gets the basic MQTT credentials password, that will be used by the provisioned device
    /// @return Basic MQTT credentials password
    inline const char* Get_Credentials_Password() const {
      return m_credPassword;
    }

    /// @brief Sets the basic MQTT credentials password, that will be used by the provisioned device
    /// @param password Basic MQTT credentials password
    inline void Set_Credentials_Password(const char *password) {
      m_credPassword = password;
    }

    /// @brief Gets the basic MQTT credentials clientID, that will be used by the provisioned device
    /// @return Basic MQTT credentials clientID
    inline const char* Get_Credentials_Client_ID() const {
      return m_credClientID;
    }

    /// @brief Sets the basic MQTT credentials clientID, that will be used by the provisioned device
    /// @param clientID Basic MQTT credentials clientID
    inline void Set_Credentials_Client_ID(const char *clientID) {
      m_credClientID = clientID;
    }

    /// @brief Gets the public X.509 certificate hash, that will be used by the provisioned device
    /// @return Public X.509 certificate hash
    inline const char* Get_Certificate_Hash() const {
      return m_hash;
    }

    /// @brief Sets the public X.509 certificate hash, that will be used by the provisioned device
    /// @param hash Public X.509 certificate hash
    inline void Set_Certificate_Hash(const char *hash) {
      m_hash = hash;
    }

    /// @brief Gets the string containing the used credentials type that decides which provisioning method is actually used,
    /// by the Provision_Callback and therefore decides what response we will receive from the server
    /// @return String containing the used credentials type
    inline const char* Get_Credentials_Type() const {
      return m_credentialsType;
    }

  private:
    processFn   m_cb;               // Callback to call
    const char  *m_deviceKey;       // Device profile provisioning key
    const char  *m_deviceSecret;    // Device profile provisioning secret
    const char  *m_deviceName;      // Device name the provisioned device should have
    const char  *m_accessToken;     // Access token supplied by the device, if it should not be generated by the server instead
    const char  *m_credUsername;    // MQTT credential username, if the MQTT basic credentials method is used
    const char  *m_credPassword;    // MQTT credential password, if the MQTT basic credentials method is used
    const char  *m_credClientID;    // MQTT credential clientID, if Mthe QTT basic credentials method is used
    const char  *m_hash;            // X.509 certificate hash, if the X.509 certificate authentication method is used
    const char  *m_credentialsType; // Credentials type we are requesting from the server, nullptr for the default option (Credentials generated by the ThingsBoard server)
};

#endif // Provision_Callback_h
