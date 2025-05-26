#include "t3f/t3f.h"
#include "t3net/t3net.h"
#include "leaderboard.h"

static int _t3f_leaderboard_offset = 0;
static int _t3f_leaderboard_prime_factor = 0;
static char * _t3f_leaderboard_title = NULL;
static char * _t3f_leaderboard_version = NULL;
static int _t3f_leaderboard_upload_scores = false;

bool t3f_initialize_leaderboards(const char * section, const char * title, const char * version, const char * user_key_url, const char * user_name_url, const char * update_url, const char * query_url)
{
  const char * val;

  _t3f_leaderboard_title = strdup(title);
  if(!_t3f_leaderboard_title)
  {
    goto fail;
  }
  _t3f_leaderboard_version = strdup(version);
  if(!_t3f_leaderboard_version)
  {
    goto fail;
  }

  /* set up default URLs */
  if(user_key_url)
  {
    val = al_get_config_value(t3f_user_data, section, "user_key_url");
    if(!val)
    {
      al_set_config_value(t3f_user_data, section, "user_key_url", user_key_url);
    }
  }
  if(user_name_url)
  {
    val = al_get_config_value(t3f_user_data, section, "user_name_url");
    if(!val)
    {
      al_set_config_value(t3f_user_data, section, "user_name_url", user_name_url);
    }
  }
  if(update_url)
  {
    val = al_get_config_value(t3f_user_data, section, "update_url");
    if(!val)
    {
      al_set_config_value(t3f_user_data, section, "update_url", update_url);
    }
  }
  if(query_url)
  {
    val = al_get_config_value(t3f_user_data, section, "query_url");
    if(!val)
    {
      al_set_config_value(t3f_user_data, section, "query_url", query_url);
    }
  }
  return true;

  fail:
  {
    t3f_deinitialize_leaderboards();
    return false;
  }
}

void t3f_deinitialize_leaderboards(void)
{
  if(_t3f_leaderboard_title)
  {
    free(_t3f_leaderboard_title);
    _t3f_leaderboard_title = NULL;
  }
  if(_t3f_leaderboard_version)
  {
    free(_t3f_leaderboard_version);
    _t3f_leaderboard_version = NULL;
  }
}

void t3f_define_leaderboard_obfuscation(int offset, int prime_factor)
{
  _t3f_leaderboard_offset = offset;
  _t3f_leaderboard_prime_factor = prime_factor;
}

static int _t3f_obfuscate_score(int score)
{
  if(_t3f_leaderboard_prime_factor != 0)
  {
    score *= _t3f_leaderboard_prime_factor;
  }
  score += _t3f_leaderboard_offset;

  return score;
}

void t3f_enable_leaderboard_uploads(bool onoff)
{
  _t3f_leaderboard_upload_scores = onoff;
}

static int _t3f_unobfuscate_score(int score)
{
  score -= _t3f_leaderboard_offset;
  if(_t3f_leaderboard_prime_factor != 0)
  {
    score /= _t3f_leaderboard_prime_factor;
  }

  return score;
}

static bool _t3f_verify_score(int score)
{
  if(_t3f_leaderboard_prime_factor != 0)
  {
    score -= _t3f_leaderboard_offset;
    if(score % _t3f_leaderboard_prime_factor == 0)
    {
      return true;
    }
    return false;
  }
  return true;  
}

static void _transform_text(const char * text, char * buffer, int max)
{
  int i;

  memset(buffer, 0, max);
  for(i = 0; i < strlen(text) && i < max - 1; i++)
  {
    if((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'A' && text[i] <= 'Z') || (text[i] >= 'a' && text[i] <= 'z'))
    {
      buffer[i] = text[i];
    }
    else
    {
      buffer[i] = '_';
    }
  }
}

void _get_key_text(const char * prefix, const char * version, const char * mode, const char * option, char * buffer)
{
  char version_text[32];
  char mode_text[64];
  char option_text[256];

  _transform_text(version, version_text, 32);
  _transform_text(mode, mode_text, 64);
  _transform_text(option, option_text, 256);

  sprintf(buffer, "%s_%s_%s_%s", prefix, version_text, mode_text, option_text);
}

void t3f_store_leaderboard_score(const char * section, const char * mode, const char * option, int flags, int score, const char * extra)
{
  char key_text[256];
  const char * val;
  char buf[256];
  bool store = false;

  score = _t3f_obfuscate_score(score);
  _get_key_text("score", _t3f_leaderboard_version, mode, option, key_text);
  val = al_get_config_value(t3f_user_data, section, key_text);
  if(val)
  {
    if((flags & T3F_LEADERBOARD_FLAG_ASCEND))
    {
      if(score < atoi(val))
      {
        store = true;
      }
    }
    else if(score > atoi(val))
    {
      store = true;
    }
  }
  else
  {
    store = true;
  }
  if(store)
  {
    sprintf(buf, "%d", score);
    al_set_config_value(t3f_user_data, section, key_text, buf);
    sprintf(buf, "%s_stored", key_text);
    al_set_config_value(t3f_user_data, section, buf, "false");
    if(extra)
    {
      _get_key_text("extra", _t3f_leaderboard_version, mode, option, key_text);
      al_set_config_value(t3f_user_data, section, buf, extra);
    }
  }
}

int t3f_retrieve_leaderboard_score(const char * section, const char * mode, const char * option)
{
  char key_text[256];
  const char * val;

  _get_key_text("score", _t3f_leaderboard_version, mode, option, key_text);
  val = al_get_config_value(t3f_user_data, section, key_text);
  if(val)
  {
    return _t3f_unobfuscate_score(atoi(val));
  }
  return -1;
}

const char * t3f_retrieve_leaderboard_extra(const char * section, const char * mode, const char * option)
{
  char key_text[256];

  _get_key_text("extra", _t3f_leaderboard_version, mode, option, key_text);
  return al_get_config_value(t3f_user_data, section, key_text);
}

void t3f_set_leaderboard_user_name(const char * section, const char * name)
{
  const char * val;

  if(name)
  {
    val = al_get_config_value(t3f_user_data, section, "user_name");
    if(!val || strcmp(val, name))
    {
      al_set_config_value(t3f_user_data, section, "user_name", name);
      al_set_config_value(t3f_user_data, section, "user_name_stored", "false");
    }
  }
}

const char * t3f_get_leaderboard_user_key(const char * section)
{
  const char * val;
  const char * user_name;
  const char * url;
  char * new_key = NULL;

  url = al_get_config_value(t3f_user_data, section, "user_key_url");
  if(!url)
  {
    goto fail;
  }

  user_name = al_get_config_value(t3f_user_data, section, "user_name");
  val = al_get_config_value(t3f_user_data, section, "user_key");
  if(!val)
  {
    new_key = t3net_get_new_leaderboard_user_key(url, user_name);
    if(new_key)
    {
      al_set_config_value(t3f_user_data, section, "user_key", new_key);
      free(new_key);
      if(user_name)
      {
        al_remove_config_key(t3f_user_data, section, "user_key_stored");
      }
    }
    val = al_get_config_value(t3f_user_data, section, "user_key");
  }
  return val;

  fail:
  {
    return NULL;
  }
}

bool t3f_submit_leaderboard_score(const char * section, const char * mode, const char * option)
{
  char key_text[256];
  char extra_key_text[256];
  char buf[256];
  const char * val;
  const char * url;
  const char * user_key;
  int score;
  bool ret = false;

  _get_key_text("score", _t3f_leaderboard_version, mode, option, key_text);
  sprintf(buf, "%s_stored", key_text);
  val = al_get_config_value(t3f_user_data, section, buf);
  if(val && !strcasecmp(val, "false"))
  {
    url = al_get_config_value(t3f_user_data, section, "update_url");
    if(!url)
    {
      goto fail;
    }
    user_key = t3f_get_leaderboard_user_key(section);
    if(!user_key)
    {
      goto fail;
    }
    val = al_get_config_value(t3f_user_data, section, key_text);
    if(!val)
    {
      goto fail;
    }
    score = atoi(val);
    if(_t3f_verify_score(score))
    {
      _get_key_text("extra", _t3f_leaderboard_version, mode, option, extra_key_text);
      val = al_get_config_value(t3f_user_data, section, key_text);
      ret = t3net_upload_score(url, _t3f_leaderboard_title, _t3f_leaderboard_version, mode, option, user_key, score, val);
    }
    if(ret)
    {
      sprintf(buf, "%s_stored", key_text);
      al_remove_config_key(t3f_user_data, section, buf);
    }
    return ret;
  }

  fail:
  {
    return false;
  }
}

typedef struct
{

  char * section;
  char * mode;
  char * option;
  int entries;
  int flags;

} _T3F_LEADERBOARD_LOAD_INFO;

static void _t3f_destroy_leaderboard_load_info(_T3F_LEADERBOARD_LOAD_INFO * ip)
{
  if(ip)
  {
    if(ip->mode)
    {
      free(ip->mode);
    }
    if(ip->option)
    {
      free(ip->option);
    }
    free(ip);
  }
}

static _T3F_LEADERBOARD_LOAD_INFO * _t3f_create_leaderboard_load_info(const char * section, const char * mode, const char * option, int entries, int flags)
{
  _T3F_LEADERBOARD_LOAD_INFO * ip;

  ip = malloc(sizeof(_T3F_LEADERBOARD_LOAD_INFO));
  if(!ip)
  {
    goto fail;
  }
  memset(ip, 0, sizeof(_T3F_LEADERBOARD_LOAD_INFO));
  ip->section = strdup(section);
  if(!ip->section)
  {
    goto fail;
  }
  ip->mode = strdup(mode);
  if(!ip->mode)
  {
    goto fail;
  }
  ip->option = strdup(option);
  if(!ip->option)
  {
    goto fail;
  }
  ip->entries = entries;
  ip->flags = flags;

  return ip;

  fail:
  {
    _t3f_destroy_leaderboard_load_info(ip);
    return NULL;
  }
}

static void * _t3f_actually_get_leaderboard(void * arg)
{
	_T3F_LEADERBOARD_LOAD_INFO * load_info = (_T3F_LEADERBOARD_LOAD_INFO *)arg;
  const char * url;
  T3NET_LEADERBOARD * ret = NULL;
  int i;

	if(arg)
	{
    /* submit unsubmitted score */
    if(_t3f_leaderboard_upload_scores)
    {
      if(t3f_get_leaderboard_user_key(load_info->section))
      {
        t3f_submit_leaderboard_score(load_info->section, load_info->mode, load_info->option);
      }
    }
    /* get the leaderboard */
    url = al_get_config_value(t3f_user_data, load_info->section, "query_url");
    if(url)
    {
      ret = t3net_get_leaderboard(url, _t3f_leaderboard_title, _t3f_leaderboard_version, load_info->mode, load_info->option, load_info->entries, load_info->flags & T3F_LEADERBOARD_FLAG_ASCEND);
      for(i = 0; i < ret->entries; i++)
      {
        ret->entry[i]->score = _t3f_unobfuscate_score(ret->entry[i]->score);
      }
      free(load_info);
      return ret;
    }
	}
	return NULL;
}

T3F_LEADERBOARD * t3f_get_leaderboard(const char * section, const char * mode, const char * option, int entries, int flags, bool threaded)
{
  _T3F_LEADERBOARD_LOAD_INFO * load_info = NULL;
  T3F_LEADERBOARD * lp = NULL;

  lp = malloc(sizeof(T3F_LEADERBOARD));
  if(!lp)
  {
    goto fail;
  }
  memset(lp, 0, sizeof(T3F_LEADERBOARD));
  lp->loader = t3f_create_object_loader();
  if(!lp->loader)
  {
    goto fail;
  }
  load_info = _t3f_create_leaderboard_load_info(section, mode, option, entries, flags);
  if(!load_info)
  {
    goto fail;
  }
  lp->data = t3f_load_object(lp->loader, _t3f_actually_get_leaderboard, NULL, load_info, threaded);

  return lp;

  fail:
  {
    if(lp)
    {
      t3f_destroy_leaderboard(lp);
    }
    if(load_info)
    {
      _t3f_destroy_leaderboard_load_info(load_info);
    }
    return NULL;
  }
}

void t3f_destroy_leaderboard(T3F_LEADERBOARD * lp)
{
  if(lp)
  {
    if(lp->data)
    {
      t3net_destroy_leaderboard(lp->data);
    }
    if(lp->loader)
    {
      t3f_destroy_object_loader(lp->loader);
    }
    free(lp);
  }
}

bool t3f_update_loaderboard(T3F_LEADERBOARD * lp)
{
  if(t3f_object_ready(lp->loader))
  {
    lp->data = t3f_fetch_object(lp->loader);
    return true;
  }
  return false;
}
