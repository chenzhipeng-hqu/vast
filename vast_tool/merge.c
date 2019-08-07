#include <stdio.h>
#include <stdint.h>

uint32_t hex2u32(const char *str)
{
  const char *pCh = str;
  uint32_t data = 0;
  if(pCh[1] == 'x' || pCh[1] == 'X')
  {
    //skip 0x
    pCh += 2;
  }
  while(*pCh)
  {
    if(data & 0xf0000000)
    {
      // support max 32 bit data.
      break;
    }
    if(*pCh >= 'a' && *pCh <= 'f')
    {
      data <<= 4;
      data += *pCh - 'a' + 10;
    }
    else if(*pCh >= 'A' && *pCh <= 'F')
    {
      data <<= 4;
      data += *pCh - 'A' + 10;
    }
    else if(*pCh >= '0' && *pCh <= '9')
    {
      data <<= 4;
      data += *pCh - '0';
    }
    else
    {
      break;
    }
    pCh ++;
  }
  return data;
}

uint32_t str2u32(const char *str)
{
  const char *pCh = str;
  uint32_t data = 0;
  if(str[1] == 'x' || str[1] == 'X')
  {
    return hex2u32(str);
  }
  while(*pCh)
  {
    if(data > 0xffffffff / 10)
    {
      // support max 32 bit data.
      break;
    }
    if(*pCh >= '0' && *pCh <= '9')
    {
      data *= 10;
      data += *pCh - '0';
    }
    else
    {
      break;
    }
    pCh ++;
  }
  return data;
}

int main(int argc, char *argv[])
{
  FILE *pLoader, *pApp, *pMerge;
  int ch;
  int cnt;
  int locate;
  int ret = 1;

  if(argc < 5)
  {
    printf("Error.\n");
    printf("merge <bootloader.bin> <app.bin> <merge.bin> <0x4000>\n");
    return 2;
  }
  locate = str2u32(argv[4]);
  pLoader = fopen(argv[1], "rb");
  if(pLoader)
  {
    fseek(pLoader, 0, SEEK_END);
    cnt = ftell(pLoader);
    if(cnt <= locate)
    {
      fseek(pLoader, 0, SEEK_SET);

      pApp = fopen(argv[2], "rb");
      if(pApp)
      {
        pMerge = fopen(argv[3], "wb");
        if(pMerge)
        {
          cnt = 0;
          while(1)
          {
            ch = fgetc(pLoader);
            if(ch == EOF)
            {
              break;
            }
            fputc(ch, pMerge);
            cnt ++;
          }
          for(;cnt<locate; cnt++)
          {
            fputc(0xff, pMerge);
          }
          while(1)
          {
            ch = fgetc(pApp);
            if(ch == EOF)
            {
              break;
            }
            fputc(ch, pMerge);
            cnt++;
          }
          fclose(pMerge);
          printf("write %s[%d B] OK\n", argv[3], cnt);
          ret = 0;
        }
        else
        {
          printf("open %s failed\n", argv[3]);
        }
        fclose(pApp);
      }
      else
      {
        printf("open %s failed\n", argv[2]);
      }
    }
    fclose(pLoader);
  }
  else
  {
    printf("open %s failed\n", argv[1]);
  }
  return ret;
}

