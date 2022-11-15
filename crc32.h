

#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>


/*
  Код взят из https://ru.wikibooks.org, статья про избыточный код
 */


/*
  Name  : CRC-32
  Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 байт (2 147 483 647 бит) - обнаружение
   одинарных, двойных, пакетных и всех нечетных ошибок
*/



uint_least32_t Crc32(const unsigned char * buf, size_t len);

#endif /* CRC32_H */
