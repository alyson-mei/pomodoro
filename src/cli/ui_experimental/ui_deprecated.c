



// void pad_content(const char *s, char *buf, const char *border, int len) {
//     size_t slen = strlen(s);
//     size_t blen = strlen(border);   // could be >1 for UTF-8

//     // left border
//     memcpy(buf, border, blen);
//     buf += blen;

//     // left padding
//     memset(buf, ' ', len);
//     buf += len;

//     // copy the string
//     memcpy(buf, s, slen);
//     buf += slen;

//     // right padding
//     memset(buf, ' ', len);
//     buf += len;

//     // right border
//     memcpy(buf, border, blen);
//     buf += blen;

//     *buf = '\0';  // null terminator
// }
