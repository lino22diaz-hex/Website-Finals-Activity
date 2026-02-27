const nextButton = document.querySelector('.next-btn');
const video = document.querySelector('.hero-video');

const movieList = [
    'image/hero-1.mp4',
    'image/hero-2.mp4',
    'image/hero-3.mp4',
    'image/hero-4.mp4'
];

let index = 0;

nextButton.addEventListener('click', function(){

    index++;

    if (index >= movieList.length){
        index = 0;
    }

    video.src = movieList[index];
    video.load();
    video.play();
});


const navItems = document.querySelectorAll('.nav-item');

document.addEventListener("click", function(e) {

    navItems.forEach(item => {
        const dropdown = item.querySelector('.dropdown');

        if (item.contains(e.target)) {
            dropdown.classList.toggle('show');
        } else {
            dropdown.classList.remove('show');
        }
    });

});

const contactToggle = document.querySelector('.contact-toggle');
const contactMenu = document.querySelector('.contact-menu');

document.addEventListener("click", function(e) {

    if (contactToggle.contains(e.target)) {
        contactMenu.classList.toggle("show");
    } else {
        contactMenu.classList.remove("show");
    }

});