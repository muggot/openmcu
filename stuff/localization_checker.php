<?php // localization checker - simple php script by kay27, for OpenMCU-ru project, (c)2014

// Most actual translation that will be used as reference:
// Наиболее актуальный перевод, который будет использован в качестве эталона:
$reference='ru';

// Here is a paradox: we compare token list with 'ru', but we want missing tokens
// to be pulled out from 'en' (because English is more "international"):
// Тут парадокс: сравниваем список токенов с 'ru', но хотим брать недостающие
// токены из 'en' (потому что английский язык более "международный"):
$pull_out_from='en';
//$pull_out_from='ru';

$res_dir='../openmcu-ru/resource';

$lf="\n"; if(strtolower(substr(PHP_OS, 0, 3))==='win') $lf="\r\n";

$localizations=my_scan($res_dir);
if(!in_array($reference,$localizations))
  my_die("We need a reference ($reference) to continue. Для работы требуется эталон ($reference).$lf");

$tokens=my_tokens("$res_dir/".get_locale_name($reference));
if(count($tokens)<10)
  my_die("Something's wrong: we have < 10 tokens. Что-то не так: у нас < 10 токенов.$lf");

foreach($localizations as $language)
{
  if($language==$reference) continue;
  $l_tokens=my_tokens("$res_dir/".get_locale_name($language));
  my_diff_handler($reference, $language, my_compare($tokens, $l_tokens));
}

check_presence('../openmcu-ru/Makefile',$localizations);
check_presence('../openmcu-ru/Makefile.in',$localizations);
check_presence('../openmcu-ru/mcu.cxx',$localizations);
check_template_html($res_dir.'/template.html',$localizations);

////////////////////////////////////////////////////////////////////////////////////////////////////

function get_locale_name($language) { return "locale_$language.js"; }

function my_token_grabber($tokens)
{
  global $res_dir, $pull_out_from;
  $f=file($res_dir.'/'.get_locale_name($pull_out_from));
  $r=Array();
  $grabbing=false;
  foreach($f as $str)
  {
    $newtoken=(substr($str,0,9)==='window.l_');
    if($newtoken)
    {
      $token=$str;
      $ep=strpos($str,'=');
      if($ep!==false) $token=substr($str,0,$ep);
      $token=str_replace("\t",'',trim($token));
      $grabbing=in_array($token,$tokens);
    }
    if($grabbing) $r[]=$str;
  }
  return $r;
}

function my_diff_handler($reference, $language, $diff)
{
  global $lf;
  list($missing,$excess)=$diff;
  $missing_count=count($missing);
  $excess_count=count($excess);
  if(($missing_count==0)&&($excess_count==0))
  {
    my_echo("Tokens $reference|$language identical. Токены $reference|$language идентичны.$lf");
    return;
  }
  if($missing_count)
  {
    my_echo("Missing tokens in '$language': Токены, отсутствующие в '$language':$lf" . my_token_dump($missing));
    file_put_contents(get_locale_name($language).'.missing.txt', join('',my_token_grabber($missing)));
  }
  if($excess_count)
  {
    my_echo("Excess tokens in '$language': Лишние токены в '$language':$lf" . my_token_dump($excess));
  }
}

function my_token_dump($tokens)
{
  global $lf;
  ksort($tokens);
  $s='';
  for($i=0;$i<count($tokens);$i++)
  {
    $s.= ($i+1) . ")\t" . $tokens[$i] . $lf;
  }
  return $s;
}

function my_scan($dir)
{
  $localizations=Array();
  $d=opendir($dir);
  while(($s=readdir($d))!==false)
  {
    if(is_dir("$dir/$s")) continue;
    if((substr($s,0,7)==='locale_') && (substr($s,-3)==='.js'))
    {
      $localizations[]=strtolower(substr($s,7,strlen($s)-10));
    }
  }
  closedir($d);
  return $localizations;
}

function my_tokens($filename)
{
  global $lf;
  $tokens=Array();
  foreach(file($filename) as $l => $str)
  {
    if(substr($str,0,9)==='window.l_')
    {
      $token=$str;
      $ep=strpos($str,'=');
      if($ep!==false) $token=substr($str,0,$ep);
      $token=str_replace("\t",'',trim($token));
      if(in_array($token,$tokens))
      {
        my_echo("Duplicate token $token in $filename. Дубль токена $token в $filename.$lf");
        continue;
      }
      $tokens[]=$token;
    }
  }
  return $tokens;
}

function my_compare($tokens, $testee)
{
  $missing=Array();
  foreach($tokens as $token)
  {
    $index=array_search($token,$testee);
    if($index===false) $missing[]=$token;
    else unset($testee[$index]);
  }
  sort($missing);
  sort($testee);
  return Array($missing,$testee);
}

function my_die($s)
{
  my_echo($s); die();
}

function my_echo($s)
{
  if(strtolower(substr(PHP_OS, 0, 3))==='win')
  {
    echo iconv('utf-8','cp866',$s);
    return;
  }
  echo $s;
}

function check_presence($filename, $localizations)
{
  global $lf;
  $f=file_get_contents($filename);
  foreach($localizations as $language)
  {
    $lang_filename=get_locale_name($language);
    if(strpos($f, $lang_filename)===false)
    {
      my_echo("$filename does not contain $lang_filename. $filename не содержит $lang_filename.$lf");
    }
  }
}

function check_template_html($filename, $localizations)
{
  global $lf;
  $f=file($filename);
  foreach($f as $k => $s)
  {
    if(strpos($s, 'LOCALIZATIONS_AVAILABLE'))
    {
      $q=explode("'",$s);
      $w=explode('|',strtolower($q[1]));
      if(count($w)<2) my_echo("Warning, $filename:$k: list too short. Внимание, $filename:$k список слишком мал.$lf");
      else
      {
        $wrf=false;
        foreach($localizations as $l)
          if(!in_array($l,$w))
          {
            my_echo("Language '$l' not listed in template.html. Язык '$l' не перечислен в template.html.$lf");
            $wrf=true;
          }
        if(!$wrf) my_echo("All languages are listed in template.html. Все языки перечислены в template.html.$lf");
        return;
      }
    }
  }
  my_echo("Localization list not found in template.html. Список локализаций не найден в template.html.$lf");
}
