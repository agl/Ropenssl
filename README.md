# openssl

##### *Toolkit for Encryption, Signatures and Certificates Based on OpenSSL*

[![Build Status](https://travis-ci.org/jeroenooms/openssl.svg?branch=master)](https://travis-ci.org/jeroenooms/openssl)
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jeroenooms/openssl?branch=master&svg=true)](https://ci.appveyor.com/project/jeroenooms/openssl)
[![Coverage Status](https://codecov.io/github/jeroenooms/openssl/coverage.svg?branch=master)](https://codecov.io/github/jeroenooms/openssl?branch=master)
[![CRAN_Status_Badge](http://www.r-pkg.org/badges/version/openssl)](http://cran.r-project.org/package=openssl)
[![CRAN RStudio mirror downloads](http://cranlogs.r-pkg.org/badges/openssl)](http://cran.r-project.org/web/packages/openssl/index.html)

> Bindings to OpenSSL libssl and libcrypto, plus custom SSH
  pubkey parsers. Supports RSA, DSA and NIST curves P-256, P-384 and P-521.
  Cryptographic signatures can either be created and verified manually or via x509
  certificates. AES block cipher is used in CBC mode for symmetric encryption; RSA
  for asymmetric (public key) encryption. High-level envelope functions combine
  RSA and AES for encrypting arbitrary sized data. Other utilities include key
  generators, hash functions (md5, sha1, sha256, etc), base64 encoder, a secure
  random number generator, and 'bignum' math methods for manually performing

## Hello World

Download and verify an SSL certrificate:

```r
library(openssl)
cert <- download_ssl_cert("www.r-project.org")
cert_verify(cert, ca_bundle())
print(cert)
as.list(cert[[1]])
```

Encrypt a secret message using someone's RSA public key:

```r
# Generate test keys
key <- rsa_keygen()
pubkey <- as.list(key)$pubkey

# Encrypt tempkey using receivers public RSA key
secret <- charToRaw("TTIP is evil")
ciphertext <- rsa_encrypt(secret, pubkey)

# Receiver decrypts secret from private her RSA key
rawToChar(rsa_decrypt(ciphertext, key))
```

Create a signature using your RSA private key:

```r
# Sign a file with your private key
myfile <- system.file("DESCRIPTION")
sig <- signature_create(myfile, key = key)

# Others can verify form your public key
signature_verify(myfile, sig, pubkey = pubkey)
```

## Installation

Binary packages for __OS-X__ or __Windows__ can be installed directly from CRAN:

```r
install.packages("openssl")
```

Installation from source on Linux requires [`openssl`](http://openssl.org/source). On __Debian__ or __Ubuntu__ use [libssl-dev](https://packages.debian.org/testing/libssl-dev):

```
sudo apt-get install -y libssl-dev
```

On __Fedora__, __CentOS or RHEL__ use [openssl-devel](https://apps.fedoraproject.org/packages/openssl-devel):

```
sudo yum install openssl-devel
````

On __OS-X__ we need [openssl](https://github.com/Homebrew/homebrew-core/blob/master/Formula/openssl.rb) from homebrew, which may be installed as:

```
brew install openssl
```

To check which version you are running (run in a fresh terminal):

```
openssl version -a
```
