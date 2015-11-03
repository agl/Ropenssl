#include <R.h>
#include <Rinternals.h>
#include "apple.h"
#include "utils.h"
#include <openssl/pem.h>
#include <openssl/bn.h>

int password_cb(char *buf, int max_size, int rwflag, void *ctx){
  if(!ctx)
    error("No password callback supplied.");

  SEXP cb = (SEXP) ctx;
  int len;

  /* case where password is a hardcoded string */
  if(isString(cb)){
    len = LENGTH(STRING_ELT(cb, 0));
    len = MIN(len, max_size);
    memcpy(buf, CHAR(STRING_ELT(cb, 0)), len);
    return len;
  }

  /* case where password is an R function */
  if(isFunction(cb)){
    int err;
    SEXP call = PROTECT(LCONS(cb, LCONS(mkString("Please enter private key passphrase: "), R_NilValue)));
    SEXP res = PROTECT(R_tryEval(call, R_GlobalEnv, &err));
    if(err || !isString(res)){
      UNPROTECT(2);
      error("Password callback did not return a string value");
    }
    len = LENGTH(STRING_ELT(res, 0));
    len = MIN(len, max_size);
    memcpy(buf, CHAR(STRING_ELT(res, 0)), len);
    UNPROTECT(2);
    return len;
  }
  error("Callback must be string or function");
}

SEXP R_parse_pem_name(SEXP input){
  BIO *mem = BIO_new_mem_buf(RAW(input), LENGTH(input));
  char *name;
  char *header;
  unsigned char *data;
  long len;
  PEM_read_bio(mem, &name, &header, &data, &len);
  BIO_free(mem);
  Rprintf("name: %s\n", name);
  SEXP res = allocVector(RAWSXP, len);
  memcpy(RAW(res), data, len);
  return res;
}

SEXP R_parse_pem_key(SEXP input){
  BIO *mem = BIO_new_mem_buf(RAW(input), LENGTH(input));
  EVP_PKEY *pkey = PEM_read_bio_PrivateKey(mem, NULL, password_cb, NULL);
  BIO_free(mem);
  bail(!!pkey);
  unsigned char *buf = NULL;
  int len = i2d_PrivateKey(pkey, &buf);
  bail(len);
  SEXP res = PROTECT(allocVector(RAWSXP, len));
  memcpy(RAW(res), buf, len);
  setAttrib(res, R_ClassSymbol, mkString("key"));
  setAttrib(res, install("type"), ScalarInteger(EVP_PKEY_type(pkey->type)));
  free(buf);
  UNPROTECT(1);
  return res;
}

SEXP R_parse_pem_pubkey(SEXP input){
  BIO *mem = BIO_new_mem_buf(RAW(input), LENGTH(input));
  EVP_PKEY *pkey = PEM_read_bio_PUBKEY(mem, &pkey, password_cb, NULL);
  BIO_free(mem);
  bail(!!pkey);
  unsigned char *buf = NULL;
  int len = i2d_PUBKEY(pkey, &buf);
  bail(len);
  SEXP res = PROTECT(allocVector(RAWSXP, len));
  memcpy(RAW(res), buf, len);
  setAttrib(res, R_ClassSymbol, mkString("pubkey"));
  setAttrib(res, install("type"), ScalarInteger(EVP_PKEY_type(pkey->type)));
  free(buf);
  UNPROTECT(1);
  return res;
}

SEXP R_parse_pem_cert(SEXP input){
  BIO *mem = BIO_new_mem_buf(RAW(input), LENGTH(input));
  X509 *cert = PEM_read_bio_X509(mem, NULL, password_cb, NULL);
  unsigned char *buf = NULL;
  int len = i2d_X509(cert, &buf);
  bail(len);
  SEXP res = PROTECT(allocVector(RAWSXP, len));
  memcpy(RAW(res), buf, len);
  setAttrib(res, R_ClassSymbol, mkString("x509"));
  UNPROTECT(1);
  free(buf);
  return res;
}

/* Legacy pubkey format */
SEXP R_parse_pem_pkcs1(SEXP input){
  BIO *mem = BIO_new_mem_buf(RAW(input), LENGTH(input));
  RSA *rsa = PEM_read_bio_RSAPublicKey(mem, NULL, password_cb, NULL);
  bail(!!rsa);
  unsigned char *buf = NULL;
  int len = i2d_RSA_PUBKEY(rsa, &buf);
  bail(len);
  SEXP res = allocVector(RAWSXP, len);
  memcpy(RAW(res), buf, len);
  free(buf);
  return res;
}

SEXP R_parse_der_pubkey(SEXP input){
  const unsigned char *ptr = RAW(input);
  EVP_PKEY *pkey = d2i_PUBKEY(NULL, &ptr, LENGTH(input));
  bail(!!pkey);
  unsigned char *buf = NULL;
  int len = i2d_PUBKEY(pkey, &buf);
  bail(len);
  SEXP res = PROTECT(allocVector(RAWSXP, len));
  memcpy(RAW(res), buf, len);
  setAttrib(res, R_ClassSymbol, mkString("pubkey"));
  setAttrib(res, install("type"), ScalarInteger(EVP_PKEY_type(pkey->type)));
  free(buf);
  UNPROTECT(1);
  return res;
}

SEXP R_parse_der_key(SEXP input){
  BIO *mem = BIO_new_mem_buf(RAW(input), LENGTH(input));
  EVP_PKEY *pkey = d2i_PrivateKey_bio(mem, NULL);
  BIO_free(mem);
  bail(!!pkey);
  unsigned char *buf = NULL;
  int len = i2d_PrivateKey(pkey, &buf);
  bail(len);
  SEXP res = PROTECT(allocVector(RAWSXP, len));
  memcpy(RAW(res), buf, len);
  setAttrib(res, R_ClassSymbol, mkString("key"));
  setAttrib(res, install("type"), ScalarInteger(EVP_PKEY_type(pkey->type)));
  free(buf);
  UNPROTECT(1);
  return res;
}

SEXP R_parse_der_cert(SEXP input){
  const unsigned char *ptr = RAW(input);
  X509 *cert = d2i_X509(NULL, &ptr, LENGTH(input));
  bail(!!cert);
  unsigned char *buf = NULL;
  int len = i2d_X509(cert, &buf);
  bail(len);
  SEXP res = allocVector(RAWSXP, len);
  memcpy(RAW(res), buf, len);
  free(buf);
  return res;
}
