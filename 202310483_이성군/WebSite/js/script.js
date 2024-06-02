document.getElementById('contact-form').addEventListener('submit', function(event) {
    event.preventDefault(); // 폼 제출 기본 동작 막기
  
    // 로딩 바 표시
    document.getElementById('loading-bar').style.display = 'block';
  
    // 폼 데이터 수집
    const formData = new FormData(this);
  
    // 예시로 콘솔에 폼 데이터를 출력
    for (let [key, value] of formData.entries()) {
      console.log(key, value);
    }
  
    // 실제로 서버에 데이터를 보내려면 아래 주석을 해제하고 서버 URL을 설정
    // fetch('/submit-form', {
    //   method: 'POST',
    //   body: formData
    // }).then(response => response.json())
    //   .then(data => {
    //     console.log(data);
    //     setTimeout(() => {
    //       document.getElementById('loading-bar').style.display = 'none';
    //       document.getElementById('success-message').style.display = 'block';
    //     }, 500);
    //   }).catch(error => {
    //     console.error('Error:', error);
    //     alert('There was an error submitting the form');
    //   });
  
    // 성공 메시지 표시 (서버 요청이 없는 경우, 이 코드를 유지하세요)
    setTimeout(() => {
      document.getElementById('loading-bar').style.display = 'none';
      document.getElementById('success-message').style.display = 'block';
    }, 500);
  });
  
  const scrollContainer = document.getElementById('scroll-container');

  window.addEventListener('scroll', () => {
    if (window.scrollY > 0) { // 페이지가 스크롤되면
      scrollContainer.classList.add('shrink'); // 요소의 클래스에 'shrink' 클래스 추가
    } else {
      scrollContainer.classList.remove('shrink'); // 스크롤이 맨 위에 있으면 'shrink' 클래스 제거
    }
  });

  function showContent(contentId) {
    // 모든 컨텐츠 숨기기
    var contents = document.getElementsByClassName('content');
    for (var i = 0; i < contents.length; i++) {
        contents[i].classList.remove('active-content');
    }
    // 선택된 컨텐츠 보여주기
    document.getElementById(contentId).classList.add('active-content');
}