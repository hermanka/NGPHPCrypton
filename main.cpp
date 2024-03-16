#include <iostream>
#include <phpcpp.h>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <glob.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <functional>
#include <dirent.h>

/**
 *  tell the compiler that the get_module is a pure C function
 */

using namespace std;
string variable_name;
int counter = 0;

void GetReqDirs(const std::string &path, std::vector<string> &files, const bool showHiddenDirs = false) {
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir(path.c_str());
    if (dpdf != NULL)
    {
        while ((epdf = readdir(dpdf)) != NULL)
        {
            if (showHiddenDirs ? (epdf->d_type == DT_DIR && string(epdf->d_name) != ".." && string(epdf->d_name) != ".") : (epdf->d_type == DT_DIR && strstr(epdf->d_name, "..") == NULL && strstr(epdf->d_name, ".") == NULL))
            {
                GetReqDirs(path + epdf->d_name + "/", files, showHiddenDirs);
            }
            if (epdf->d_type == DT_REG)
            {
                files.push_back(path + epdf->d_name);
            }
        }
    }
    closedir(dpdf);
}

bool CheckSubstring(std::string firstString, std::string secondString) {
    if (secondString.size() > firstString.size())
        return false;

    for (size_t i = 0; i < firstString.size(); i++) {
        size_t j = 0;
        // If the first characters match
        if (firstString[i] == secondString[j]) {
            int k = i;
            while (firstString[i] == secondString[j] && j < secondString.size()) {
                j++;
                i++;
            }
            if (j == secondString.size())
                return true;
            else // Re-initialize i to its original value
                i = k;
        }
    }
    return false;
}


string random_string() {
    /*<< We first define the characters that we're going
         to allow.  This is pretty much just the characters
         on a standard keyboard.
    >>*/
    std::string tmp_s;
    std::string chars(
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    /*<< We use __random_device as a source of entropy, since we want
         passwords that are not predictable.
    >>*/
    boost::random::random_device rng;
    /*<< Finally we select 16 random characters from the
         string and print them to cout.
    >>*/
    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
    for (int i = 0; i < 16; ++i) {
        tmp_s += chars[index_dist(rng)];
    }
    return tmp_s;
}

bool in_array(const string &needle, const vector<string> &haystack) {
    int max = haystack.size();

    if (max == 0)
        return false;

    for (int i = 0; i < max; i++)
        if (haystack[i] == needle)
            return true;
    return false;
}

std::vector<std::string> glob(const std::string &pattern) {
    glob_t glob_result = {0}; // zero initialize

    // do the glob operation
    int return_value = ::glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    if (return_value != 0)
        throw std::runtime_error(std::strerror(errno));

    // collect all the filenames into a std::vector<std::string>
    // using the vector constructor that takes two iterators
    std::vector<std::string> filenames(
        glob_result.gl_pathv, glob_result.gl_pathv + glob_result.gl_pathc);

    // cleanup
    globfree(&glob_result);

    // done
    return filenames;
}

class Cryptix : public Php::Base {
private:
    /**
     *  Example property
     *  @var    int
     */
    int _value = 0;

public:
    /**
     *  C++ constructor and destructor
     */
    Cryptix() = default;
    virtual ~Cryptix() = default;

    /** 
     *  Static method
     *
     *  A static method also has no 'this' pointer and has
     *  therefore a signature identical to regular functions
     *
     *  @param  params      Parameters passed to the method
     */

    //obfuscation
    static string obfuskasi(string codeAwal)     {
        vector<string> variable_names_before;
        vector<string> variable_names_after;
        // vector<string> function_names_before;
        // vector<string> function_names_after;
        vector<string> forbidden_variables =
            {"$GLOBALS", "$_SERVER", "$_GET", "$_POST", "$_FILES",
             "$_COOKIE", "$_SESSION", "$_REQUEST", "$_ENV", "$php_errormsg",
             "$http_response_header", "$argc", "$argv"
            };

        // function obfuscation pending support
        // vector<string> forbidden_functions_tmp = Php::call("get_defined_functions","");
        // vector<string> forbidden_functions = forbidden_functions_tmp["internal"];


        string file_contents = codeAwal;
        bool lock = false;
        string lock_quote = "";
        for (size_t i = 0; i < file_contents.size(); i++) {
            // check if there are quotation marks
            string comparestring(1, file_contents.at(i));
            if ((comparestring.compare("'") || file_contents.at(i) == '"')) {
                // if first quote
                if (lock_quote == "") {
                    // remember quotation mark
                    lock_quote = file_contents.at(i);
                    lock = true;
                }
                else if (comparestring.compare(lock_quote)) {
                    lock_quote = "";
                    lock = false;
                }
            }
            //cout <<"check quatation marks passed"<<endl;

            // detect variables
            if (!lock && file_contents.at(i) == '$') {
                int start = i;
                // detect variable variable names

                if (file_contents.at(i + 1) == '$') {
                    start++;
                    i++;
                }

                int end = 1;
                // find end of variable name
                while (isalpha(file_contents.at(start + end)) || isdigit(file_contents.at(start + end)) || file_contents.at(start + end) == '_'){
                    end++;
                }
                // extract variable name
                variable_name = file_contents.substr(start, end);

                if (variable_name == "$"){
                    continue;
                }

                // check if variable name is allowed
                if (!in_array(variable_name, forbidden_variables)) {

                    // check if variable name already has been detected
                    if (!in_array(variable_name, variable_names_before))
                    {
                        variable_names_before.push_back(variable_name);

                        string new_variable_name = "";
                        do
                        {
                            new_variable_name = random_string();
                        } while (in_array(new_variable_name, variable_names_after));
                        variable_names_after.push_back(new_variable_name);
                    }
                }
            }
            
            // detect function-definitions
            // the third condition checks if the symbol before 'function' is neither a character nor a number
            // function name pending to support


            // if (!lock && file_contents.substr(i, 8) == "function" && (!isalpha(file_contents[i - 1]) && !isdigit(file_contents[i - 1]))) {
            //     // find end of function name
            //     int end = file_contents.find('(', i);
            //     // extract function name and remove possible spaces on the right side
            //     string function_name_helper = file_contents.substr((i + 9), (end - i - 9));
            //     boost::trim_right(function_name_helper);
            //     string function_name = function_name_helper;
            //     // check if function name is allowed
            //     if (in_array(function_name, forbidden_functions))
            //     {
            //     }
            //     else
            //     {
            //         if (!in_array(function_name, function_names_before))
            //         {
            //             function_names_before.push_back(function_name);
            //             //cout << "nama fungsi yang terdeteksi:" << function_name << endl;

            //             // generate random name for variable
            //             string new_function_name = "";
            //             do
            //             {

            //                 new_function_name = random_string();

            //             } while (in_array(new_function_name, function_names_after));
            //             function_names_after.push_back(new_function_name);
            //         }
            //     }
            // }
            //cout <<"check function "<< i << "passed"<<endl;
        }


        // this array contains prefixes and suffixes for string literals which
        // may contain variable names.
        // if string literals as a return of functions should not be changed
        // remove the last two inner arrays of $possible_pre_suffixes
        // this will enable correct handling of situations like
        // - $func = 'getNewName'; echo $func();
        // but it will break variable variable names like
        // - ${getNewName()}

        // Two-dimensional key
        map<int, map<string, string>> possible_pre_suffixes;
        possible_pre_suffixes[0] = {{"prefix", "= '"}, {"suffix", "'"}};
        possible_pre_suffixes[1] = {{"prefix", "=\""}, {"suffix", "\""}};
        possible_pre_suffixes[2] = {{"prefix", "='"}, {"suffix", "'"}};
        possible_pre_suffixes[3] = {{"prefix", "=\""}, {"suffix", "\""}};
        possible_pre_suffixes[4] = {{"prefix", "rn \""}, {"suffix", "\""}};
        possible_pre_suffixes[5] = {{"prefix", "rn '"}, {"suffix", "'"}};

        // replace variable name

        for (size_t i = 0; i < variable_names_before.size(); i++)
        {
            string dolar = "$";
            string helper = dolar.append(variable_names_after[i]);
            boost::algorithm::replace_all(file_contents, variable_names_before[i], helper);
            string name = variable_names_before[i].substr(1);

            for (size_t j = 0; j < possible_pre_suffixes.size(); j++)
            {
                string helpera = possible_pre_suffixes[j]["prefix"].append(name).append(possible_pre_suffixes[j]["suffix"]);
                string helperb = possible_pre_suffixes[j]["prefix"].append(variable_names_after[i]).append(possible_pre_suffixes[j]["suffix"]);
                boost::algorithm::replace_all(file_contents, helpera, helperb);
            }
        }
        //cout <<"replace variable name passed"<<endl;


        // replace funciton names
        // obfuscate function pending support

        // for (size_t i = 0; i < function_names_before.size(); i++)
        // {
        //     // cout << file_contents.size()<<endl;
        //     //cout << "before ++> "<<function_names_before[i] << " %% "<< "after ++> "<<function_names_after[i]<<endl;
        //     boost::algorithm::replace_all(file_contents, function_names_before[i], function_names_after[i]);
        //     //cout <<"process ["<<i<< "] replace function name passed"<<endl;
            
        // }
        //cout <<"replace function name passed"<<endl;

        return file_contents;
    }

    static void obfus(Php::Parameters &params) {
        std::string type = "aes-256-cbc";
        std::string path = params[0];

        vector<string> listfile;
        vector<string> res;

        GetReqDirs(path, listfile);

        for (string file : listfile){
            if (CheckSubstring(file, ".php"))
            {
                res.push_back(file);
            }
        }

        string input, coba, str3, akhir, enc_code;
        string name[res.size()];
        string gabungan = "potong";
        string nama,hasil;
        vector<string> penanda;
        for (size_t i = 0; i < res.size(); i++){
            akhir = gabungan + to_string(i);
            penanda.push_back(akhir);
        }
        int cnt = 0;
        int i = 0;
        for (string data : res){

            string nama = Php::call("basename", data);
            name[cnt] = nama;
            string hasil = Php::call("file_get_contents", data);
            // Php::out << Php::call("file_put_contents", res.at(i) + ".orignal", hasil) << std::endl; // need to remove
            cnt++;
            //hitung = to_string(cnt);

            if ((size_t)cnt < res.size()){
                input += hasil + "\n" + penanda.at(i) + "\n";
                i++;
            } else if ((size_t)cnt == res.size()) {
                input += hasil;
            }
             coba = obfuskasi(input);
            
        }

        int counter = 0;
        size_t npos = 0;
        for (size_t i = 0; i < res.size(); i++) {
            if (i < res.size() - 1) {
                std::size_t pos = coba.find(penanda.at(i)); // position of "end" in coba
                npos = pos - counter;
                str3 = coba.substr(counter, npos);

                // Php::out << Php::call("file_put_contents", res.at(i) + ".obfuskasi", str3) << std::endl; // need to remove
                enc_code = "<?php PHPCrypton::decode('" + openssl_enc(type, str3) + "'); ?>";
                Php::out << Php::call("file_put_contents", res.at(i), enc_code) << std::endl;

                counter = pos + penanda.at(i).length();
            } else {
                std::size_t pos = coba.length();
                str3 = coba.substr(counter, pos);
                // Php::out << Php::call("file_put_contents", res.at(i) + ".obfuskasi", str3) << std::endl;
                enc_code = "<?php PHPCrypton::decode('" + openssl_enc(type, str3) + "'); ?>";
                Php::out << Php::call("file_put_contents", res.at(i), enc_code) << std::endl;
            }

        }
    }

    static void decrypt(Php::Parameters &params) {
        // @todo add implementation
        std::string type = "aes-256-cbc";
        std::string msg = params[0];

        // :TODO kondisi jika tidak ada close tag
        std::string plain_code = openssl_dec(type, msg);
        std::string clean_code = Php::call("rtrim", plain_code);

        // get close tag
        std::string end_code = Php::call("substr", clean_code, -2);
        std::string sanitize_code = Php::call("substr", clean_code, 0, -2);

        // standard code is omitting close php tag
        std::string standard_code;
        if (end_code == "?>") {
            // remove closing tag
            standard_code = sanitize_code;
        } else {
            standard_code = clean_code;
        }

        std::string code = " ?>" + standard_code;
        Php::out << Php::eval(code) << std::endl;
    }

    static std::string openssl_enc(std::string method, std::string data) {

        std::string firstkey = "Lk5Uz3slx3BrAghS1aaW5AYgWZRV0tIX5eI0yPchFz4=";
        std::string secondkey = "EZ44mFi3TlAey1b2w4Y7lVDuqO+SRxGXsa7nctnr/JmMrA2vN6EJhrvdVZbxaQs5jpSe34X3ejFK/o9+Y5c83w==";

        // std::string data =  params[0];
        std::string first_key = Php::call("base64_decode", firstkey);
        std::string second_key = Php::call("base64_decode", secondkey);

        // std::string method = "aes-256-cbc";
        std::string iv_length = Php::call("openssl_cipher_iv_length", method);
        std::string iv = Php::call("openssl_random_pseudo_bytes", iv_length);

        // more secure pseudo-random in php7+
        //std::string iv = Php::call("random_bytes", iv_length);

        Php::Value openssl_raw_data = Php::constant("OPENSSL_RAW_DATA");

        std::string first_encrypted = Php::call("openssl_encrypt", data, method, first_key, openssl_raw_data, iv);
        std::string second_encrypted = Php::call("hash_hmac", "sha512", first_encrypted, second_key, true);

        std::string output = Php::call("base64_encode", iv + second_encrypted + first_encrypted);
        return output;
        // Php::out << output << std::endl;
    }

    static std::string openssl_dec(std::string method, std::string cipher) {
        // static void aes_dec(Php::Parameters &params){

        std::string firstkey = "Lk5Uz3slx3BrAghS1aaW5AYgWZRV0tIX5eI0yPchFz4=";
        std::string secondkey = "EZ44mFi3TlAey1b2w4Y7lVDuqO+SRxGXsa7nctnr/JmMrA2vN6EJhrvdVZbxaQs5jpSe34X3ejFK/o9+Y5c83w==";

        std::string first_key = Php::call("base64_decode", firstkey);
        std::string second_key = Php::call("base64_decode", secondkey);

        std::string mix = Php::call("base64_decode", cipher);
        // std::string method = "aes-256-cbc";
        int iv_length = Php::call("openssl_cipher_iv_length", method);
        std::string iv = Php::call("substr", mix, 0, iv_length);

        std::string second_encrypted = Php::call("substr", mix, iv_length, 64);
        std::string first_encrypted = Php::call("substr", mix, iv_length + 64);

        Php::Value openssl_raw_data = Php::constant("OPENSSL_RAW_DATA");

        std::string data = Php::call("openssl_decrypt", first_encrypted, method, first_key, openssl_raw_data, iv);

        std::string second_encrypted_new = Php::call("hash_hmac", "sha512", first_encrypted, second_key, true);

        if (Php::call("hash_equals", second_encrypted, second_encrypted_new)) {
            return data;
        } else {
            return "0";
        }
    }
};

extern "C"
{

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("phpcrypton", "1.0");

        // Static Class
        Php::Class<Cryptix> myCrypt("PHPCrypton");
        myCrypt.method<&Cryptix::decrypt>("decode");        
        myCrypt.method<&Cryptix::obfus>("obfuscate");
        extension.add(std::move(myCrypt));

        return extension;
    }
}
