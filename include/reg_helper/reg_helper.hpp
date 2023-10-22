#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <optional>
#include <string_view>

/**
 * @brief 
 * 
 * @tparam ACCESS_MASK 
 */
template<REGSAM ACCESS_MASK>
class reg_helper
{
private:
    std::string_view m_reg_key;
    HKEY m_key_handle;

public:
    reg_helper(HKEY parent_key, std::string_view reg_key) :
        m_reg_key(reg_key)
    {
        auto status = RegOpenKeyExA(parent_key, reg_key.data(), 0, ACCESS_MASK, &m_key_handle);
        if (status != ERROR_SUCCESS)
        {
            throw std::runtime_error("Exception occurred while calling RegOpenKeyExA!");
        }
    }

    virtual ~reg_helper() noexcept
    {
        RegCloseKey(m_key_handle);
    }

    /**
     * @brief Get the value of a registry key
     * 
     * @tparam T 
     * @param key_name 
     * @return std::optional<T> 
     */
    template<typename T>
    std::optional<T> get_value(const std::string_view key_name) const
    {
        DWORD size = sizeof(T);
        T output;

        if (RegGetValueA(m_key_handle, nullptr, key_name.data(), RRF_RT_REG_BINARY | RRF_RT_REG_DWORD | RRF_RT_REG_QWORD, nullptr, &output, &size) != ERROR_SUCCESS)
        {
            return std::nullopt;
        }
        return output;
    }

    /**
     * @brief Get the value of a multi value registry key
     * 
     * @tparam  
     * @param key_name 
     * @return std::optional<std::vector<std::string>> 
     */
    template<>
    std::optional<std::vector<std::string>> get_value(const std::string_view key_name) const
    {
        DWORD size {0};

        if (RegGetValueA(m_key_handle, nullptr, key_name.data(), RRF_RT_REG_MULTI_SZ, nullptr, nullptr, &size) != ERROR_SUCCESS)
        {
            return std::nullopt;
        }

        auto data = std::make_unique<char[]>(size);
        if (RegGetValueA(m_key_handle, nullptr, key_name.data(), RRF_RT_REG_MULTI_SZ, nullptr, data.get(), &size) != ERROR_SUCCESS)
        {
            return std::nullopt;
        }

        std::vector<std::string> output;
        for (auto ptr = data.get(); ptr < data.get() + size - 1; ptr += (strlen(ptr) + 1))
        {
            output.push_back(ptr);
        }
        return output;
    }

    /**
     * @brief Get the value of a string registry key
     * 
     * @tparam  
     * @param key_name 
     * @return std::optional<std::string> 
     */
    template<>
    std::optional<std::string> get_value(const std::string_view key_name) const
    {
        DWORD size {0};

        if (RegGetValueA(m_key_handle, nullptr, key_name.data(), RRF_RT_REG_SZ, nullptr, nullptr, &size) != ERROR_SUCCESS)
        {
            return std::nullopt;
        }

        auto data = std::make_unique<char[]>(size);
        if (RegGetValueA(m_key_handle, nullptr, key_name.data(), RRF_RT_REG_SZ, nullptr, data.get(), &size) != ERROR_SUCCESS)
        {
            return std::nullopt;
        }
        return {{ data.get(), size }};
    }

    /**
     * @brief Set a registry key value, any type of data should be supported as long as it can be sized by "sizeof".
     * 
     * @tparam T 
     * @param key_name 
     * @param value 
     * @return true Successfully set registry value.
     * @return false Failure in settings registry key, use FormatMessage to get the exception message.
     */
    template<typename T>
    bool set_value(const std::string_view key_name, T value) const
    {
        DWORD type = REG_BINARY;
        if constexpr (sizeof(T) == 4)
        {
            type = REG_DWORD;
        }
        else if constexpr (sizeof(T) == 8)
        {
            type = REG_QWORD;
        }

        return RegSetKeyValueA(m_key_handle, nullptr, key_name.data(), type, &value, sizeof(T)) == ERROR_SUCCESS;
    }

    /**
     * @brief Set a string registry value
     * 
     * @param key_name 
     * @param str 
     * @return true Successfully set registry value.
     * @return false Failure in settings registry key, use FormatMessage to get the exception message.
     */
    bool set_value(const std::string_view key_name, const std::string& str) const
    {
        return RegSetKeyValueA(m_key_handle, nullptr, key_name.data(), REG_SZ, str.c_str(), str.size() + 1) == ERROR_SUCCESS;
    }

    /**
     * @brief Set a multiple string registry key.
     * 
     * @param key_name 
     * @param strs 
     * @return true Successfully set registry value.
     * @return false Failure in settings registry key, use FormatMessage to get the exception message.
     */
    bool set_value(const std::string_view key_name, const std::vector<std::string>& strs) const
    {
        std::size_t alloc_size {1};
        for (const auto& str : strs)
        {
            alloc_size += str.size() + 1;
        }

        auto value = std::make_unique<char[]>(alloc_size);
        auto it = strs.begin();
        for (auto ptr = value.get(); ptr < value.get() + alloc_size - 1; ptr += it->size() + 1, it++)
        {
            std::memcpy(ptr, it->c_str(), it->size() + 1);
        }
        value.get()[alloc_size - 1] = '\0';

        return RegSetKeyValueA(m_key_handle, nullptr, key_name.data(), REG_MULTI_SZ, value.get(), alloc_size) == ERROR_SUCCESS;
    }

};