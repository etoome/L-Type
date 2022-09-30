#pragma once

#include <string>
#include <vector>

/* Generate a random string.
 */
std::string genRandomStr(unsigned int length) noexcept;

/* Get key from file.
 */
std::string getKey(const std::string& path);

/* Generate a hash from a string.
 */
std::string hash(const std::string& str);

/* Generate a hashed signature from a string.
 */
std::string genSignature(const std::string& str);
