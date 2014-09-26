<?php

 //simple and useful when you want to split trace.txt into many files by threads
 //known problem: very slow
 //known problem: multiple-line messages will be cut after 1st line

  $filename='trace.txt';
  $thread_id_tab_pos=1;
  $output_thread_mask='_thread_%N_%ID.txt';
  $width=60;
  $thread_digits=4;

  $lf="\n"; if(strtolower(substr(PHP_OS, 0, 3))==='win') $lf="\r\n";

  $fids=Array(); $f=fopen($filename,'r'); $q=0;
  while(($s=fgets($f,4096))!==false)
  { $q++;
    $t=split("\t",$s);
    if(count($t) > 1)
    {
      $ts=substr($t[$thread_id_tab_pos],0,50);
      $cp = strrpos($ts, ':');
      if($cp !== false)
      {
        $fid=substr($t[$thread_id_tab_pos],$cp+1);
        $fid0=split(' ',$fid);
        $fid=$fid0[0];
        if(!in_array($fid, $fids)) {$fids[]=$fid; echo "New thread $fid started at " . trim($t[0]) . ", line $q$lf"; }
      }
    }
  }
  $step=$q;

  echo str_pad('',$width,'-') . "{$lf}Threads total: " . count($fids) . $lf;
  fseek($f,0);

  $q=0; foreach($fids as &$id)
  { $q++; $fn=str_pad($q, $thread_digits, '0',STR_PAD_LEFT);
    $r[$id]=str_replace("%N",$fn,str_replace("%ID",$id,$output_thread_mask));
  }


  $q=0; while(($s=fgets($f,4096))!==false)
  { $q+=$width; if($q>=$step) { $q-=$step; echo '*'; }
    foreach($fids as &$id)
      if (strpos($s,':'.$id)!==false)
        file_put_contents($r[$id],$s,FILE_APPEND);
  }

  fclose($f);
  echo $lf;
