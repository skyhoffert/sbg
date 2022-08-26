///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Filename:      sbg.c
// Description:   Implmentation file for sbg (s Browser GUI) library.
//
// Additional Notes:
// ------------------
// None.
//
// Development History:
//  Date         Author        Description of Change
// ------       --------      -----------------------
// 2022-08-26   Sky Hoffert   Initial release.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "sbg.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>

#include <openssl/sha.h>

#define WS_RESPONSE_MSG "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "

static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
unsigned char * base64_encode(const unsigned char *src, size_t len,
			      size_t *out_len)
{
	unsigned char *out, *pos;
	const unsigned char *end, *in;
	size_t olen;
	int line_len;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */
	if (olen < len)
		return NULL; /* integer overflow */
	out = malloc(olen);
	if (out == NULL)
		return NULL;

	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[((in[0] & 0x03) << 4) |
					      (in[1] >> 4)];
			*pos++ = base64_table[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
		line_len += 4;
	}

	if (line_len)
		*pos++ = '\n';

	*pos = '\0';
	if (out_len)
		*out_len = pos - out;
	return out;
}

void _sbg_send(int fd, const char* data)
{
    char buf[1024];
    memset(buf, 0, 1024);

    int len = strlen(data);

    if (len > 125)
    {
        printf("Couldn't send, too long.\n");
        return;
    }

    buf[0] = 0x81; // 0b1000 0001, final fragment, opcode 1 (text)
    buf[1] = 0x00 + len; // 0b1000 0001, MASK true, len 1 byte

    /*
    unsigned int mask = rand();

    unsigned char maskc[4];

    maskc[0] = (mask >> 24) & 0x0ff;
    maskc[1] = (mask >> 16) & 0x0ff;
    maskc[2] = (mask >> 8 ) & 0x0ff;
    maskc[3] = (mask >> 0 ) & 0x0ff;

    buf[2] = maskc[0]; // MASK OCT 0
    buf[3] = maskc[1]; // MASK OCT 1
    buf[4] = maskc[2]; // MASK OCT 2
    buf[5] = maskc[3]; // MASK OCT 3

    for (int i = 0; i < len; i++)
    {
        buf[6+i] = data[i] ^ maskc[i % 4]; // payload data (masked)
    }
    */
    for (int i = 0; i < len; i++)
    {
        buf[2+i] = data[i];
    }

    printf("Sending:\n    ");
    for (int i = 0; i < strlen(buf); i++)
    {
        printf("0x%x ", (unsigned char) buf[i]);
    }
    printf("\n");

    write(fd, buf, strlen(buf));
}

void _sbg_parse(const char* data)
{
    int slen = strlen(data);
    int len = data[1] & 0x7f;

    printf("data length: %d\n", len);

    char mask[4];
    mask[0] = data[2];
    mask[1] = data[3];
    mask[2] = data[4];
    mask[3] = data[5];

    printf("Message: ");
    for (int i = 0; i < slen-6; i++)
    {
        printf("%c", data[6+i] ^ mask[i % 4]);
    }
    printf("\n");
}

int _verify_ws(const char* m)
{
    int get = 0;
    int http11 = 0;
    int uws = 0;

    int b = 0;
    int keyidx = -1;

    while (m[b] != 0)
    {
        if (strncmp(m+b, "GET", 3) == 0)
        {
            get = 1;
        }
        else if (strncmp(m+b, "HTTP/1.1", 8) == 0)
        {
            http11 = 1;
        }
        else if (strncmp(m+b, "Upgrade: websocket", 18) == 0)
        {
            uws = 1;
        }
        else if (strncmp(m+b, "Sec-WebSocket-Key: ", 19) == 0)
        {
            keyidx = b + 19;
        }

        b++;
    }

    if (get == 1 && http11 == 1 && uws == 1)
    {
        return keyidx;
    }

    return 0 - get - http11 - uws;
}

void* _sbg_thread(void* vargp)
{
    sbg* s = (sbg*) vargp;

    srand(time(NULL));

    fd_set fds;
    struct timeval tv;

    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);

    char buf[1024];

    int ws_verified = 0;
    int keyidx = -1;

    int counter = 0;

    while (s->_quit_flag == 0)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        FD_ZERO(&fds);
        FD_SET(s->_servfd, &fds);

        int retval = select(s->_servfd+1, &fds, NULL, NULL, &tv);

        if (retval == -1)
        {
            printf("Select error.\n");
        }
        else if (retval == 0)
        {
            printf(".");
            fflush(stdout);
        }
        else
        {
            s->_sockfd = accept(s->_servfd, (struct sockaddr*) &client, &len);

            if (s->_sockfd < 0)
            {
                printf("Bad sockfd.\n");
                s->_sockfd = -1;
            }
        }

        if (s->_sockfd >= 0)
        {
            while (s->_quit_flag == 0)
            {
                tv.tv_sec = 0;
                tv.tv_usec = 100000;

                counter += ws_verified;

                FD_ZERO(&fds);
                FD_SET(s->_sockfd, &fds);

                int retval = select(s->_sockfd+1, &fds, NULL, NULL, &tv);

                if (retval == -1)
                {
                    printf("Select error.\n");
                }
                else if (retval == 0)
                {
                    printf("_");
                    fflush(stdout);
                }
                else
                {
                    memset(buf, 0, 1024);
                    retval = read(s->_sockfd, buf, 1024);

                    if (retval == 0)
                    {
                        close(s->_sockfd);
                        s->_sockfd = -1;
                    }

                    printf("got msg(%d):\n", retval);

                    printf("%s", buf);

                    if (ws_verified == 1)
                    {
                        _sbg_parse(buf);

                        continue;
                    }

                    retval = _verify_ws(buf);
                    if (retval > 0)
                    {
                        keyidx = retval;

                        ws_verified = 1;

                        char key[32];
                        char fullkey[128];
                        memset(key, 0, 32);
                        memset(fullkey, 0, 128);

                        sscanf(buf + keyidx, "%s", key);

                        sprintf(fullkey, "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11", key);

                        printf("got key: %s\n", key);

                        char hash[128];
                        SHA1((unsigned char*) fullkey, (ssize_t) strlen(fullkey), (unsigned char*) hash);

                        char* encoded;
                        size_t outlen;
                        encoded = (char*)base64_encode((unsigned char*) hash, strlen(hash), &outlen);

                        sprintf(buf, "%s%s\r\n\r\n", WS_RESPONSE_MSG, encoded);

                        free(encoded);

                        printf("sending: %s\n", buf);

                        write(s->_sockfd, buf, strlen(buf));
                    }
                }

                // After connected for 2 seconds, send data.
                if (counter >= 20)
                {
                    counter = 0;
                    continue;

                    _sbg_send(s->_sockfd, "TEST");
                }

                if (strlen(s->msg) > 0)
                {
                    _sbg_send(s->_sockfd, s->msg);
                    memset(s->msg, 0, 1024);
                }
            }
        }
    }

    return NULL;
}

int sbg_init(sbg* s)
{
    s->width = 0;
    s->height = 0;

    s->_quit_flag = 0;

    s->_sockfd = -1;
    s->_servfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(s->msg, 0, 1024);

    if (s->_servfd == -1)
    {
        printf("ERR. Couldn't stand up sbg socket.\n");
        return SBG_ERR;
    }


    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(struct sockaddr_in));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SBG_PORT);

    int retval = 0;

    int enable = 1;
    retval = setsockopt(s->_servfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    if (retval < 0)
    {
        printf("ERR. Couldn't set reuseaddr option.\n");
        return SBG_ERR;
    }

    retval = bind(s->_servfd, (struct sockaddr*) &servaddr, sizeof(struct sockaddr));

    if (retval != 0)
    {
        printf("ERR. Couldn't bind sbg socket.\n");
        close(s->_servfd);
        return SBG_ERR;
    }

    retval = listen(s->_servfd, 1);
    
    if (retval != 0)
    {
        printf("ERR. Couldn't listen sbg socket.\n");
        close(s->_servfd);
        return SBG_ERR;
    }

    pthread_create(&s->_wsthread, NULL, _sbg_thread, s);

    return SBG_OK;
}

int sbg_term(sbg* s)
{
    s->_quit_flag = 1;

    pthread_join(s->_wsthread, NULL);

    close(s->_servfd);

    return SBG_OK;
}

void sbg_draw_line(const sbg* s, const sbg_pt* a, const sbg_pt* b)
{

}
