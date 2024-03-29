# ft_irc Projesi

Bu proje kendi IRC sunucunuzu oluşturmanızı sağlar. Gerçek bir IRC istemcisi kullanarak sunucunuzu bağlanıp test edebilirsiniz. İnternet, birbirleriyle etkileşimde bulunan bilgisayarların sağlam standart protokollerle yönetildiği bir ortamdır. Bu projeyi yaparak bu standartları öğrenmek her zaman faydalı olacaktır.

## Genel Kurallar

- Programınız herhangi bir durumda çökmemeli ve beklenmedik şekilde kapanmamalıdır.
- Bir Makefile dosyası teslim etmelisiniz ve bu dosya yeniden ilişkilendirme yapmamalıdır.
- Makefile dosyanız en azından `$(NAME)`, `all`, `clean`, `fclean` ve `re` kurallarını içermelidir.
- Kodunuz C++ 98 standardına uygun olmalıdır.
- C fonksiyonlarını kullanabilirsiniz ancak mümkün olduğunda C++ sürümlerini tercih etmelisiniz.
- Harici kütüphaneler ve Boost kütüphaneleri yasaktır.

## Zorunlu Kısım

- Program adı: ircserv
- Teslim edilecek dosyalar: Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp, isteğe bağlı yapılandırma dosyası
- Makefile: `NAME`, `all`, `clean`, `fclean`, `re` kuralları içermelidir.
- Argümanlar: Port numarası, bağlantı şifresi
- Harici fonksiyonlar: C++ 98'deki standart fonksiyonlar
- Libft kullanımı: Yetkilendirilmiştir
- Açıklama: C++ 98'de bir IRC sunucusu geliştirmelisiniz. İstemci geliştirmemelisiniz ve sunucu-sunucu iletişimini işlememelisiniz.

---