document.getElementById("createBtn").addEventListener("click", async () => {
    const filename = document.getElementById("filename1").value;

    // 把 filename 傳給後端
    const response = await fetch("http://localhost:8080/file/create", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({ name: filename })
    });

    const data = await response.json();
    console.log(data);
});




document.getElementById("deleteBtn").addEventListener("click", async () => {
    const filename = document.getElementById("filename2").value;

    // 把 filename 傳給後端
    const response = await fetch("http://localhost:8080/file/delete", {
        method: "DELETE",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({ name: filename })
    });

    const data = await response.json();
    console.log(data);
});


document.getElementById("printBtn").addEventListener("click", async () => {
    try {
        const response = await fetch("http://localhost:8080/file/printLRU", {
            method: "POST",   // ✅ 必須是 POST
            headers: {
                "Content-Type": "application/json"
            }
        });

        if (!response.ok) {
            console.error("HTTP error", response.status);
            return;
        }

        const data = await response.json();
        // console.log(data);
        const allFiles = data.files;
        // console.log(allFiles);
        // for (let i = 0; i < data.cache_size; i++) {
        //     let filename = allFiles[i].filename + ".txt";
        //     console.log(filename);
        // }

        // 把資料渲染到網頁
        const output = document.getElementById("output");
        // if (output) output.textContent = JSON.stringify(data, null, 2);
        output.innerHTML = "";
        for (let i = 0; i < data.cache_size; i++) {
            let filename = allFiles[i].filename + ".txt";
            // console.log(filename);
            const li = document.createElement("li");
            li.textContent = filename;
            output.appendChild(li);
        }

    } catch (err) {
        console.error("Fetch error:", err);
    }
});



document.getElementById("printBtn2").addEventListener("click", async () => {
    try {
        const response = await fetch("http://localhost:8080/file/printAll", {
            method: "POST",   // ✅ 必須是 POST
            headers: {
                "Content-Type": "application/json"
            }
        });

        if (!response.ok) {
            console.error("HTTP error", response.status);
            return;
        }

        const data = await response.json();
        console.log(data);

        // 把資料渲染到網頁
        const output = document.getElementById("output2");
        output.innerHTML = "";
        for (let i = 0; i < data.length; i++) {
            let filename = data[i];
            // console.log(filename);
            const li = document.createElement("li");
            li.textContent = filename;
            output.appendChild(li);
        }

    } catch (err) {
        console.error("Fetch error:", err);
    }
});
