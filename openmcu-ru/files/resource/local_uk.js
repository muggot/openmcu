window.l_welcome = "OpenMCU-ru";
window.l_status = "Стан";

window.l_connections = "З’єднання";
window.l_connections_OFFLINE_PREFIX    = "<B>[Відключений] </B>";
window.l_connections_OFFLINE_SUFFIX    = ""                    ;
window.l_connections_HIDDEN_PREFIX     = "<B>[Системний] </B>"  ;
window.l_connections_HIDDEN_SUFFIX     = ""                    ;
window.l_connections_AUDIO_OUT_STR     = "Аудiовихiд"          ;
window.l_connections_AUDIO_IN_STR      = "Аудiовхiд"           ;
window.l_connections_VIDEO_OUT_STR     = "Вiдеовихiд"          ;
window.l_connections_VIDEO_IN_STR      = "Вiдеовхiд"           ;
window.l_connections_AI_NEG_ERR        = "Немає вхідного аудіо";
window.l_connections_AO_NEG_ERR        = "Немає вихідного аудіо";
window.l_connections_VI_NEG_ERR        = "Немає вхідного вiдео";
window.l_connections_VO_NEG_ERR        = "Немає вихідного вiдео";
window.l_connections_BUTTON_TEXT       = "Отримати текст"      ;
window.l_connections_BUTTON_FORUM      = "Отримати BBCode для форуму";
window.l_connections_BUTTON_HTML       = "Отримати HTML"       ;
window.l_connections_BUTTON_CLOSE      = "Приховати код"       ;
window.l_connections_CODE_TOOLTIP      = "Натисніть Ctrl+C, щоб скопіювати код";
window.l_connections_DAYS_STR          = "д."                  ;
window.l_connections_COL_NAME          = "Iм’я"                ;
window.l_connections_COL_DURATION      = "Тривалість"          ;
window.l_connections_COL_RTP           = "RTP-канал i кодек"   ;
window.l_connections_COL_PACKETS       = "Пакети"              ;
window.l_connections_COL_BYTES         = "Байти"               ;
window.l_connections_COL_KBPS          = "Кбіт/с"              ;
window.l_connections_COL_FPS           = "Кадр/с"              ;
window.l_connections_word_room         = "Конференцiя"         ;
window.l_connections_COL_LOSTPCN       = "Втрати за 60с"       ;

window.l_records = 'Відеозаписи';
window.l_control = "Управління";
window.l_rooms = "Кімнати";
window.l_invite = "Подзвонити";
window.l_invite_s = "Виклик пішов на термінал";
window.l_invite_f = "Виклик не відбувся";
window.l_settings = "Налаштування";
window.l_param_general = "Основнi";
window.l_param_registrar = 'Реєстратор';
window.l_param_conference = 'Конференції';
window.l_param_export = 'Експорт(named pipe)';
window.l_param_managing_users = "Користувачі";
window.l_param_managing_groups = "Групи";
window.l_param_control_codes = "Коди управління";
window.l_param_room_codes = "Коди кімнат";
window.l_param_h323_endpoints = "H.323-термінали";
window.l_param_sip_endpoints = "SIP-термінали";
window.l_param_rtsp = 'RTSP параметри';
window.l_param_rtsp_servers = 'RTSP сервери';
window.l_param_rtsp_endpoints = 'RTSP термінали';
window.l_param_video = "Вiдео";
window.l_param_record = "Запис";
window.l_param_h323 = "Параметри H.323";
window.l_param_sip = "Параметри SIP";
window.l_param_sip_proxy = "Облікові записи SIP";
window.l_param_access_rules = "Правила доступу";
window.l_param_receive_sound = 'Аудiо-кодеки (приймання)';
window.l_param_transmit_sound = 'Аудiо-кодеки (відправлення)';
window.l_param_receive_video = 'Вiдео-кодеки (приймання)';
window.l_param_transmit_video = 'Вiдео-кодеки (відправлення)';
window.l_param_sip_sound = 'Аудiо-кодеки';
window.l_param_sip_video = 'Вiдео-кодеки';
window.l_help = 'Допомога';
window.l_forum = 'Форум'
window.l_manual = 'Документація';
window.l_support = 'Підтримка';

window.l_info_welcome ='\
<p>\
Служба OpenMCU-ru запущена і приймає підключення.\
</p>\
<p>\
Конференція, що створена підключенням першого терміналу, спочатку працює у некерованому режимі, при цьому всі учасники конференції бачать однакову картинку.\
 У некерованому режимі OpenMCU-ru автоматично змінює розкладку екрану залежно від кількості підключених користувачів, у порядку, визначеному у файлі layouts.conf.\
 Після під’єднанння 100 користувачів всі наступні під’єднані користувачі будуть невидимими на екранах терміналів, але буде видно перших 100 користувачів.<br />\
Звук у некерованому режимі передається та приймається від всіх під’єднаних учасників.<br />\
</p>\
<p>\
Для перевірки роботи сервера також можна зателефонувати до кімнати з ім’ям \"echo\". Або в кімнату з ім’ям \"testroomN\", де N - число від 1 до 100.<br />\
</p>\
<p>\
Управління сервером здійснюється через головне меню  у горі сторінки.<br />\
Сторінка <b> Стан </b> дозволяє переглядати поточні з’єднання з сервером і параметри підключених терміналів.<br />\
На сторінці <b> Управління </b> можна перевести необхідні кімнати на ручне керування, для завдання власних розкладок конференції. <br /> \
На сторінці <b> Налаштування </b> задаються глобальні параметри роботи сервера.<br />\
Додаткову інформацію можна знайти на сторінці <b> Допомога </b> (посилання веде на офіційний сайт в Інтернет).<br />\
</p>\
<p>Нижче показано зведення по серверу.</p>\
';
window.l_welcome_logo ='\
<p>\
<b>Власний логотип</b>\
</p>\
<p>\
Цей логотип показується у мікшері при відсутності вхідного відео. Допускається завантаження BMP, JPEG(не більше 500kB), PNG, GIF.\
</p>\
';

window.l_info_invite ='\
Для під’єднанння у полі Room Name потрібно вказати ім’я конференції, до якої буде під’єднаний термінал, \
 за замовчуванням потрібно вказати ім’я room101), у полі Address \
 набрати IP адресу (або номер при використанні гейткіпера) \
 підключаємого терміналу і натиснути кнопочку Invite. \
 У результаті на термінал піде виклик.';

window.l_info_invite_f ='У запиті на підключення була допущена помилка.';
window.l_info_invite_s ='Запит на підключення успішно відправлений на віддалений термінал.';
window.l_info_rooms ='Для перегляду і управління кімнатою - клацніть по її імені.';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="Ця сторінка призначена для управління конференцією. Управління розкладкою дозволяє вибрати видимих учасників конференції та призначити їм потрібне розташування на екрані. Для кожної позиції в розкладці можна явно вказати видимого учасника конференції, або залишити позицію порожньою, або вказати, що позиція обрана для автоматичної активації по голосовій активності.<br />За автоматичну активацію по голосовій активності відповідають два режими VAD і VAD2. Якщо позиції в розкладці призначити режим VAD, то це означатиме, що позицію може зайняти будь-який термінал, з якого йде активний звуковий сигнал.<br>Заміна терміналу на зайнятій VAD позиції можлива після закінчення інтервалу Voice activation timeout.";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>Ця сторінка призначена для\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
Сторінка дозволяє переглядати вміст директорії (на сервері), \
в якій збережені записи відеоконференцій, викачувати відеозаписи, та видаляти їх.';

window.l_info_connections ='\
Сторінка дозволяє переглядати поточні з’єднання з сервером і параметри підключених терміналів.\
';
window.l_info_param_general ='Загальні параметри роботи сервера. Для скидання всіх параметрів на початкові - встановіть галочку "RESTORE DEFAULTS" і натисніть Accept.';
window.l_info_param_registrar = '';
window.l_info_param_conference ='';
window.l_info_param_managing_users ='';
window.l_info_param_managing_groups ='';
window.l_info_param_control_codes ='';
window.l_info_param_room_codes ='';
window.l_info_param_video ='Налаштування якості вихідного відео.';
window.l_info_param_record ='Налаштування якості запису.';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_sound = '';
window.l_info_param_receive_video = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'Створити кімнату'     ;
window.l_select_enter =        'Керувати кімнатою'    ;
window.l_select_record =       'Запис'                ;
window.l_select_moderated =    'Керований режим'      ;
window.l_select_moderated_yes ='Так'                  ;
window.l_select_moderated_no = 'Нi'                   ;
window.l_select_visible =      'Кількість видимих'    ;
window.l_select_unvisible =    'Кількість невидимих'  ;
window.l_select_duration =     'Тривалість роботи'    ;
window.l_select_delete =       'Видалити кiмнату'     ;

window.l_not_found = 'не знайдено';

window.l_name_accept         = 'Зберегти';
window.l_name_reset          = 'Скасувати';
window.l_name_user           = 'Користувач';
window.l_name_host           = 'Хост';
window.l_name_group          = 'Група';
window.l_name_password       = 'Пароль';
window.l_name_roomname       = "Iм'я кімнати";
window.l_name_access         = 'Доступ';
window.l_name_action         = 'Дiя';
window.l_name_code           = 'Код';
window.l_name_message        = 'Повідомлення';
window.l_name_address        = 'Адреса';
window.l_name_address_book   = 'Адресна книга';
window.l_name_save           = 'Зберегти';
window.l_name_cancel         = 'Скасувати';
window.l_name_advanced       = 'Розширені';
window.l_name_auto_create              = 'Автоматичне створення';
window.l_name_auto_delete_empty        = 'Автоматичне видалення';
window.l_name_auto_record_start        = 'Автоматичний запис';
window.l_name_recall_last_template     = 'Створити з останнім шаблоном';
window.l_name_time_limit               = 'Обмежити за часом';

window.l_name_display_name                         = "Ім'я, що відображається";
window.l_name_frame_rate_from_mcu                  = 'Частота кадрів від MCU';
window.l_name_bandwidth_from_mcu                   = 'Бітрейт від MCU, Кбіт/с';
window.l_name_bandwidth_to_mcu                     = 'Бітрейт до MCU, Кбіт/с';
window.l_name_transport                            = 'Транспортний протокол';
window.l_name_port                                 = 'Порт';

window.l_name_registrar                            = 'Реєстратор';
window.l_name_account                              = 'Обліковий запис';
window.l_name_register                             = 'Реєструвати';
window.l_name_address_sip_proxy                    = 'Адреса SIP-проксі';
window.l_name_expires                              = 'Тривалість реєстрації';
window.l_name_path                                 = 'Шлях';
window.l_name_enable                               = 'Включити';

window.l_name_registered                           = 'Зареєстровано';
window.l_name_connected                            = 'Підключений';
window.l_name_last_ping_response                   = 'Останній відгук';

window.l_name_codec                                = 'Кодек';
window.l_name_audio                                = 'Аудіо';
window.l_name_video                                = 'Відео';
window.l_name_audio_receive                        = 'Аудіо (приймання)';
window.l_name_audio_transmit                       = 'Аудіо (передача)';
window.l_name_video_receive                        = 'Відео (приймання)';
window.l_name_video_transmit                       = 'Відео (передача)';
window.l_name_audio_codec                          = 'Аудіо кодек';
window.l_name_video_codec                          = 'Відео кодек';
window.l_name_audio_codec_receive                  = 'Аудіо кодек(приймання)';
window.l_name_video_codec_receive                  = 'Відео кодек(приймання)';
window.l_name_audio_codec_transmit                 = 'Аудіо кодек(передача)';
window.l_name_video_codec_transmit                 = 'Відео кодек(передача)';
window.l_name_video_resolution                     = 'Роздільна здатність відео';

window.l_name_parameters_for_sending               = "Параметри для відправки";
window.l_name_codec_parameters                     = "Параметри кодека <br> (перевизначити отримані)";
window.l_name_default_parameters                   = "Параметри за замовчуванням";

window.l_room_invite_all_inactive_members          = 'Викликати всіх відключених учасників';
window.l_room_drop_all_active_members              = 'Відключити всіх активних учасників';
window.l_room_remove_all_inactive_members          = 'Видалити всіх відключених учасників зі списку';
window.l_room_drop_connection_with                 = 'Розірвати з’єднання з';
window.l_room_remove_from_list                     = 'Видалити зі списку';

window.l_room_deletion_text = ["",                   "Видалення конференції &laquo;%&raquo;: відключення учасників",
                                                     "Видалення конференції &laquo;%&raquo;: чекаємо, поки всі підуть",
                                                     "Видалення конференції &laquo;%&raquo;: відключення невидимих ​​системних учасників",
                                                     "Видалення конференції &laquo;%&raquo;: чекаємо, поки всі підуть",
                                                     "Конференція &laquo;%&raquo; ВИДАЛЕНА"
];
window.l_takecontrol                               = "Конференція працює у некерованому (автоматичному) режимі. Натисніть для керування конференцією вручну.";
window.l_decontrol                                 = "Ви керуєте конференцією вручну. Натисніть для переведення конференції у некерований (автоматичний) режим.";
window.l_vadsetup                                  = "Налаштування параметрів активації по голосу (VAD)";
window.l_globalmute                                = "Звук від учасників, які НЕ ВИВЕДЕНІ на екрані, ВКЛЮЧЕНИЙ. Натисніть для відключення.";
window.l_globalunmute                              = "Звук від учасників, які НЕ ВИВЕДЕНІ на екрані, ВІДКЛЮЧЕНИЙ. Натисніть для включення.";
window.l_filtermode                                = [
                                                       "Режим фільтрації зображення при зміні розміру кадрів: НАЙШВИДШИЙ."
                                                      ,"Режим фільтрації зображення при зміні розміру кадрів: ОПТИМАЛЬНИЙ (білінійна інтерполяція)."
                                                      ,"Режим фільтрації зображення при зміні розміру кадрів: НАЙБІЛЬШ ЯКІСНИЙ."
                                                     ];
window.l_videorecorder                             = "Почати запис конференції";
window.l_videorecorderstop                         = "Зупинити запис конференції";
window.l_pleasetakecontrol                         = "Команда недоступна, оскільки конференція працює у НЕКЕРОВАНОМУ (автоматичному) режимі";
window.l_decontrolmixersconfirm                    = "Ви переводите конференцію у НЕКЕРОВАНИЙ (автоматичний) режим.\r\nДодаткові відеомікшери будуть видалені, і залишиться тільки один.\r\nВи впевнені, що бажаєте продовжити?";
window.l_templatedeleteconfirm                     = "Шаблон * буде видалено";
window.l_changelogo                                = "Замінити: ";
window.l_dir_no_records                            = "У цій директорії поки немає жодного збереженого відеозапису.";
window.l_download                                  = "Завантажити";
window.l_totaldrivesize                            = "Загальний розмір дискового розділу: * ГіБ.";
window.l_recordstakesup                            = "Відеозаписи займають:               * ГіБ (%).";
window.l_freespaceleft                             = "Вільно:                             * ГіБ (%).";
window.l_delete                                    = "Видалити";
window.l_recwilldeleted                            = "Відеозапис * буде видалений!";
window.l_recwilldeleted_ok                         = "[Підтвердити]";
window.l_recwilldeleted_cancel                     = "Скасувати";
window.l_recwasdeleted                             = "Відеозапис * видалений";
window.l_filesize                                  = "Размір файлу";
window.l_resolution                                = "Розширення";
window.l_startdatetime                             = "Дата і час початку";
window.l_lock_tpl_default                          = "Відключати термінали, що відсутні у шаблоні (замкнути конференцію)";
window.l_name_force_split_video        = "Кешування і керування через браузер";
window.l_name_auto_record_stop         = 'Автоматичний запис (зупинити)';

///
window.l_restore_defaults                          = "Відновити налаштування за замовчуванням";
window.l_language                                  = "Мова";
window.l_server_id                                 = "Ідентифікатор сервера OpenMCU-ru";
window.l_default_protocol_for_outgoing_calls       = "Протокол за замовчуванням для вихідних дзвінків";
window.l_http_secure                               = "HTTPS";
window.l_http_certificate                          = "HTTPS сертифікат";
window.l_http_ip                                   = "HTTP IP-адреса";
window.l_http_port                                 = "HTTP порт";
window.l_rtp_base_port                             = "RTP початковий порт";
window.l_rtp_max_port                              = "RTP максимальний порт";
window.l_trace_level                               = "Рівень трасировки";
window.l_rotate_trace                              = "Ротація файлів трасировки при запуску";
window.l_log_level                                 = "Рівень системного журналу (логу)";
window.l_call_log_filename                         = "Файл журналу дзвінків";
window.l_room_control_event_buffer_size            = "Розмір буфера подій веб-журналу";
window.l_copy_web_log                              = "Копіювати веб-журнал у файл журналу дзвінків";
window.l_default_room                              = "Кімната за замовчуванням";
window.l_reject_duplicate_name                     = "Відхилити дзвінки терміналів з однаковими іменами";
window.l_allow_loopback_calls                      = "Дозволити дзвінки самому собі (loopback)";
///
window.l_allow_internal_calls                      = "Дозволити внутрішні дзвінки";
window.l_sip_allow_reg_without_auth                = "SIP дозволити реєстрацію без аутентифікації";
window.l_sip_allow_mcu_calls_without_auth          = "SIP дозволити дзвінки в MCU без аутентифікації";
window.l_sip_allow_internal_calls_without_auth     = "SIP дозволити внутрішні дзвінки без аутентифікації";
window.l_sip_registrar_minimum_expiration          = "SIP мінімальний інтервал реєстрації";
window.l_sip_registrar_maximum_expiration          = "SIP максимальний інтервал реєстрації";
window.l_h323_gatekeeper_enable                    = "H.323 включити гейткіпер";
window.l_h323_allow_reg_without_auth               = "H.323 дозволити реєстрацію без аутентифікації";
window.l_h323_allow_mcu_calls_without_reg          = "H.323 дозволити дзвінки в MCU без реєстрації";
window.l_h323_allow_internal_calls_without_reg     = "H.323 дозволити внутрішні дзвінки без реєстрації";
window.l_h323_allow_duplicate_aliases              = "H.323 дозволити аліаси що повторюються";
window.l_h323_gatekeeper_minimum_ttl               = "H.323 мінімальний інтервал реєстрації";
window.l_h323_gatekeeper_maximum_ttl               = "H.323 максимальний інтервал реєстрації";
///
window.l_enable_video                              = "Включити відео";
window.l_max_bit_rate                              = "Максимальний бітрейт";
window.l_tx_key_frame_period                       = "Інтервал відправки опорних кадрів";
window.l_encoding_threads                          = "Кількість потоків кодування";
window.l_encoding_cpu_used                         = "Використання процесора для кодування";
///
window.l_enable_export                             = "Включити експорт";
window.l_video_frame_rate                          = "Відео частота кадрів";
window.l_video_frame_width                         = "Відео ширина кадрів";
window.l_video_frame_height                        = "Відео висота кадрів";
window.l_audio_sample_rate                         = "Аудіо частота дискретизації";
window.l_audio_channels                            = "Аудіо кількість каналів";
window.l_video_bitrate                             = "Відео бітрейт";
window.l_audio_bitrate                             = "Аудіо бітрейт";
///
window.l_listener                                  = "Слухати (Listener)";
window.l_interface                                 = "Інтерфейс";
window.l_nat_router_ip                             = "NAT IP-адреса роутера";
window.l_treat_as_global_for_nat                   = "Обробляти як глобальні для NAT";
window.l_disable_fast_start                        = "Вимкнути Fast-Start";
window.l_disable_h245_tunneling                    = "Вимкнути H.245 Tunneling";
window.l_gk_mode                                   = "Гейткіпер: Тип пошуку";
window.l_gk_reg_ttl                                = "Гейткіпер: Тривалість реєстрації(TTL)";
window.l_gk_reg_retry_interval                     = "Гейткіпер: Інтервал спроб пошуку та реєстрації";
window.l_gk_host                                   = "Гейткіпер: Хост";
window.l_gk_username                               = "Гейткіпер: Ім'я користувача";
window.l_gk_password                               = "Гейткіпер: Пароль";
window.l_gk_room_names                             = "Гейткіпер: Аліаси (імена кімнат)";
///
window.l_server                                    = "Сервер";
window.l_server_list                               = "Список серверiв";
///
window.l_directory                                 = "Директорія";
window.l_rtp_input_timeout                         = "RTP таймаут";
window.l_received_vfu_delay                        = "Обмеження VFU (запит/сек)";
window.l_video_cache                               = "Відео кешування";
window.l_interval                                  = "інтервал";
window.l_internal_call_processing                  = "Внутрішні дзвінки";
window.l_room_auto_create_when_connecting          = "Автоматичне створення при підключенні";
window.l_enter_template_name                       = "Введіть ім'я шаблону";
window.l_disconnect                                = "Відключити";
window.l_add_to_abook                              = "Додати до адресної книги";
window.l_accounts                                  = "Облікові записи";
///
window.l_room_mute_all                             = 'Отключить звук от всех участников (микрофоны)';
window.l_room_unmute_all                           = 'Восстановить звук от всех участников (микрофоны)';
window.l_room_dial_all_members                     = 'Дозвон всем участникам';
///
window.l_param_telserver = 'Telnet сервер';
window.l_info_telserver = '';
window.l_auto_dial_delay                           = "Интервал автодозвона, с";
///
window.l_mute_new_conference_users                 = 'Mute new conference users';
window.l_unmute_new_conference_users               = 'Unmute new conference users';
