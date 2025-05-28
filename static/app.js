const filterBtn = document.querySelector(".filterBtn");
const themeBtn = document.querySelector(".themeBtn");
const sunIcon = document.querySelector("#sun");
const moonIcon = document.querySelector("#moon");
const countryCon = document.querySelector(".country-container");
const search = document.querySelector(".userInput");
const prevBtn = document.querySelector("#prevBtn");
const nextBtn = document.querySelector("#nextBtn");
const pageInfo = document.querySelector("#pageInfo");

let data;
let filteredData = []; 
let currentPage = 1;
const itemsPerPage = 20;

/*---THEME TOGGLE---*/

const applyTheme = (theme) => {
  if (theme === 'dark') {
    document.body.classList.add("dark");
    sunIcon.style.display = "block";
    moonIcon.style.display = "none";
  } else {
    document.body.classList.remove("dark");
    sunIcon.style.display = "none";
    moonIcon.style.display = "block";
  }
}

themeBtn.addEventListener("click", () => {
  document.body.classList.toggle("dark");
  const theme = document.body.classList.contains("dark") ? "dark" : "light";
  localStorage.setItem("theme", theme);
  applyTheme(theme);
});

const storedTheme = localStorage.getItem("theme") || "light";
applyTheme(storedTheme);

/*---FETCHING THE DATA AND DISPLAYING IT---*/

const fetchData = async () => {
  try {
    const res = await fetch("data.json");
    if (!res.ok) {
      throw new Error(`HTTP error! status: ${res.status}`);
    }
    data = await res.json();
    const storedRegion = localStorage.getItem("selectedRegion");
    const storedSearchTerm = localStorage.getItem("searchTerm") || '';

    if (storedRegion) {
      filterByRegion(storedRegion);
      filterBtn.value = storedRegion;
    } else {
      filteredData = [...data];
    }

    if (storedSearchTerm) {
      search.value = storedSearchTerm;
      searchCountries(storedSearchTerm);
    } else {
      displayCurrentData();
    }
  } catch (err) {
    console.error('Fetch error:', err);
  }
};

const displayFilterData = (filterData) => {
  const startIndex = (currentPage - 1) * itemsPerPage;
  const endIndex = currentPage * itemsPerPage;
  const pageData = filterData.slice(startIndex, endIndex);

  let countriesHTML = '';
  pageData.forEach(des => {
    countriesHTML += `
      <div class="country">
        <img src=${des.flags.svg} alt="" loading="lazy">
        <section class="country-detail">
          <a href="country.html?name=${encodeURIComponent(des.name)}"><h2 class="country-name">${des.name}</h2></a>
          <p><b>Population: </b>${des.population}</p>
          <p class="region-name"><b>Region: </b>${des.region}</p>
          <p><b>Capital: </b>${des.capital}</p>
        </section>
      </div>
    `;
  });
  countryCon.innerHTML = countriesHTML;
  pageInfo.textContent = `Page ${currentPage} of ${Math.ceil(filterData.length / itemsPerPage)}`;
  updatePaginationButtons(filterData);
};

/*---ADDING PAGINATION---*/

const updatePaginationButtons = (filterData) => {
  prevBtn.disabled = currentPage === 1;
  nextBtn.disabled = currentPage === Math.ceil(filterData.length / itemsPerPage);
};

const filterByRegion = (region) => {
  filteredData = data.filter(country => country.region === region);
  currentPage = 1; 
  localStorage.setItem("selectedRegion", region);
  displayCurrentData();
};

const searchCountries = (searchTerm) => {
  localStorage.setItem("searchTerm", searchTerm);
  filteredData = data.filter(des => des.name.toLowerCase().includes(searchTerm));
  currentPage = 1; 
  displayCurrentData();
};

const displayCurrentData = () => {
  displayFilterData(filteredData);
};

filterBtn.addEventListener("change", (e) => {
  const selectRegion = e.target.value;
  localStorage.removeItem("searchTerm");
  search.value = '';
  filterByRegion(selectRegion);
});

search.addEventListener("input", (e) => {
  const searchTerm = e.target.value.toLowerCase();
  searchCountries(searchTerm);
});

prevBtn.addEventListener("click", () => {
  if (currentPage > 1) {
    currentPage--;
    displayCurrentData();
  }
});

nextBtn.addEventListener("click", () => {
  const totalPages = Math.ceil(filteredData.length / itemsPerPage);
  if (currentPage < totalPages) {
    currentPage++;
    displayCurrentData();
  }
});

fetchData();